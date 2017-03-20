//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/18
// Time: 下午6:33
// To change this template use File | Settings | File Templates.
//

#include "proxy.h"

NS_HIVE_BEGIN

Proxy::Proxy(void) : RefObject(), m_pEpoll(NULL), m_pTimer(NULL),
	m_pListenerPool(NULL), m_pAcceptPool(NULL), m_pClientPool(NULL), m_pProxyLogic(NULL),
	m_desIndex(0) {
	memset(&m_listenInfo, 0 , sizeof(m_listenInfo));
}
Proxy::~Proxy(void){
	Proxy::destroy();
}

static Proxy* g_pProxy = NULL;
Proxy* Proxy::getInstance(void){
	return g_pProxy;
}
Proxy* Proxy::createInstance(void){
	if(g_pProxy == NULL){
		g_pProxy = new Proxy();
		SAFE_RETAIN(g_pProxy)
	}
	return g_pProxy;
}
void Proxy::destroyInstance(void){
    SAFE_RELEASE(g_pProxy)
}
void Proxy::onAcceptSocket(int fd, const char* ip, uint16 port, Listener* pListener){
	uint32 handle = Proxy::getInstance()->openAccept(fd, ip, port);
	if(handle == 0){
		fprintf(stderr, "Proxy::onAcceptSocket openAccept failed fd=%d ip=%s port=%d\n", fd, ip, port);
	}else{
		Proxy::getInstance()->getProxyLogic()->onReceiveAccept(handle, ip, port);
	}
}
int64 Proxy::checkAcceptIdentify(Accept* pAccept){
	fprintf(stderr, "--Proxy::checkAcceptIdentify handle=%d\n", pAccept->getHandle());
	pAccept->epollRemove();
	return -1;
}

uint32 Proxy::openPartner(uint32 handle, const char* ip, uint16 port){
	Accept* pAccept = this->getAccept(handle);
	if(NULL == pAccept){
		return 0;
	}
	uint32 clientHandle = this->openClient(ip, port);
	if(0 == clientHandle){
		return 0;
	}
	Client* pClient = this->getClient(clientHandle);
	pAccept->setPartner(pClient);
	pClient->setPartner(pAccept);
	return clientHandle;
}
SocketInformation* Proxy::getNextDestination(void){
	int size = (int)m_destinations.size();
	if(size == 0){
		return NULL;
	}
	if(m_desIndex >= size){
		m_desIndex = 0;
	}
	return &(m_destinations[m_desIndex++]);
}

uint32 Proxy::openListener(const char* ip, uint16 port, AcceptSocketFunction pFunc){
	fprintf(stderr, "--Proxy::openListener try to open Listener ip=%s port=%d\n", ip, port);
	Listener* pListener = (Listener*)m_pListenerPool->create();
	if(NULL == pListener){
		fprintf(stderr, "--Proxy::openListener NULL == pListener\n");
		return 0;
	}
	uint32 handle = pListener->getHandle();
	pListener->setEpoll(m_pEpoll);
	pListener->setSocket(ip, port);
	pListener->setAcceptSocketFunction(pFunc);
	if( !pListener->openSocket() ){
		closeListener(handle);
		fprintf(stderr, "--Proxy::openListener Listener openSocket failed\n");
		return 0;
	}
	if( !m_pEpoll->objectAdd(pListener, EPOLLIN) ){
		pListener->closeSocket();
		closeListener(handle);
		fprintf(stderr, "--Proxy::openListener Listener objectAdd to epoll failed\n");
		return 0;
	}
	fprintf(stderr, "--Proxy::openListener handle=%d fd=%d ip=%s port=%d\n", handle, pListener->getSocketFD(), ip, port);
	return handle;
}
uint32 Proxy::openAccept(int fd, const char* ip, uint16 port){
	fprintf(stderr, "--Proxy::openAccept fd=%d ip=%s port=%d\n", fd, ip, port);
	// 获取一个连接对象Accept，将对象一并加入到epoll中
	Accept* pAccept = m_pAcceptPool->create();
	if(NULL == pAccept){
		close(fd);
		fprintf(stderr, "--Proxy::openAccept create accept NULL == pAccept\n");
		return 0;
	}
	uint32 handle = pAccept->getHandle();
	pAccept->setEpoll(m_pEpoll);
	pAccept->setSocketFD( fd );
	pAccept->setSocket(ip, port);
	if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1){
		pAccept->closeSocket();
		closeAccept(handle);
		fprintf(stderr, "--Proxy::openAccept fcntl failed\n");
		return 0;
	}
	if( !m_pEpoll->objectAdd(pAccept, EPOLLIN) ){
		pAccept->closeSocket();
		closeAccept(handle);
		fprintf(stderr, "--Proxy::openAccept objectAdd failed\n");
		return 0;
	}
	pAccept->setConnectionState(CS_CONNECT_OK);
	pAccept->setTimeout(CONNECT_IDENTIFY_TIME, Proxy::checkAcceptIdentify);
	fprintf(stderr, "--Proxy::openAccept handle=%d fd=%d ip=%s port=%d\n", handle, fd, ip, port);
	return handle;
}
uint32 Proxy::openClient(const char* ip, uint16 port){
	Client* pClient = m_pClientPool->create();
	if(NULL == pClient){
		fprintf(stderr, "--Proxy::openClient create client NULL == pClient\n");
		return 0;
	}
	uint32 handle = pClient->getHandle();
	pClient->setEpoll(m_pEpoll);
	pClient->setSocket(ip, port);
	pClient->setConnectionState(CS_CONNECT_START);
	if( !pClient->connectServer() ){
		fprintf(stderr, "--Proxy::openClient Client::connectServer failed\n");
		closeClient(handle);
		return 0;
	}
	// 添加当前的socket到epoll中进行监听
	if( !m_pEpoll->objectAdd(pClient, EPOLLIN | EPOLLOUT) ){
		fprintf(stderr, "--Proxy::openClient Epoll::objectAdd failed\n");
		closeClient(handle);
		return 0;
	}
	fprintf(stderr, "--Proxy::openClient handle=%d ip=%s port=%d\n", handle, ip, port);
	return handle;
}
void Proxy::receiveClient(Client* pClient){
	pClient->setConnectionState(CS_CONNECT_OK);
	// 检查accept消息，把缓存发送到server
	pClient->clearTimer();
	Accept* pAccept = pClient->getPartner();
	if(NULL != pAccept){
		pAccept->clearTimer();
		pAccept->sendHashBufferToPartner();
	}
}

Listener* Proxy::getListener(uint32 handle){
	return m_pListenerPool->get(handle);
}
Accept* Proxy::getAccept(uint32 handle){
	return m_pAcceptPool->get(handle);
}
Client* Proxy::getClient(uint32 handle){
	return m_pClientPool->get(handle);
}

bool Proxy::closeListener(uint32 handle){
	Listener* pListener = this->getListener(handle);
	if(NULL == pListener){
		fprintf(stderr, "Proxy::closeListener Listener not found handle=%d\n", handle);
		return false;
	}
	fprintf(stderr, "Proxy::closeListener handle=%d\n", handle);
	m_pEpoll->objectRemove(pListener);
	pListener->resetData();
	return m_pListenerPool->idle(handle);
}
bool Proxy::closeAccept(uint32 handle){
	Accept* pAccept = this->getAccept(handle);
	if(NULL == pAccept){
		fprintf(stderr, "Proxy::closeAccept Accept not found handle=%d\n", handle);
		return false;
	}
	fprintf(stderr, "Proxy::closeAccept handle=%d\n", handle);
	m_pEpoll->objectRemove(pAccept);
	pAccept->resetData();
	return m_pAcceptPool->idle(handle);
}
bool Proxy::closeClient(uint32 handle){
	Client* pClient = this->getClient(handle);
	if(NULL == pClient){
		fprintf(stderr, "Proxy::closeClient Client not found handle=%d\n", handle);
		return false;
	}
	fprintf(stderr, "Proxy::closeClient handle=%d\n", handle);
	m_pEpoll->objectRemove(pClient);
	pClient->resetData();
	return m_pClientPool->idle(handle);
}

void Proxy::update(void){
	int64 timeout;
	while(1){
		timeout = m_pTimer->getWaitTimeout();
		m_pEpoll->update(timeout);
		m_pTimer->update();
	}
}
void Proxy::initialize(void){
	if(NULL == m_pEpoll){
		m_pEpoll = new Epoll();
		m_pEpoll->retain();
		if( !m_pEpoll->createEpoll() ){
			fprintf(stderr, "ERROR Proxy createEpoll failed\n");
		}
	}
	if(NULL == m_pTimer){
		m_pTimer = new Timer();
		m_pTimer->retain();
		m_pTimer->start(MAX_0824_HASH_SIZE);
	}
	if(NULL == m_pListenerPool){
		m_pListenerPool = new ListenerPool();
		m_pListenerPool->retain();
	}
	if(NULL == m_pAcceptPool){
		m_pAcceptPool = new AcceptPool();
		m_pAcceptPool->retain();
	}
	if(NULL == m_pClientPool){
		m_pClientPool = new ClientPool();
		m_pClientPool->retain();
	}
	this->initConfig();
	uint32 listenHandle = openListener(m_listenInfo.ip, m_listenInfo.port, Proxy::onAcceptSocket);
	if(listenHandle == 0){
		fprintf(stderr, "listen failed ip=%s port=%d\n", m_listenInfo.ip, m_listenInfo.port);
	}
}
void Proxy::destroy(void){
	SAFE_RELEASE(m_pEpoll)
	SAFE_RELEASE(m_pTimer)
	SAFE_RELEASE(m_pListenerPool)
	SAFE_RELEASE(m_pAcceptPool)
	SAFE_RELEASE(m_pClientPool)
}
void parse_ip_port(SocketInformation* pInfo, const std::string& ip){
	Token::TokenMap ip_port_map;
	Token::split(ip, ":", ip_port_map);
	for(auto &kv : ip_port_map){
		pInfo->setSocket(kv.first.c_str(), atoi(kv.second.c_str()));
		return;
	}
}
bool Proxy::initConfig(void){
	Token::TokenMap configMap;
	Token::readConfig("config.ini", configMap);
	// 解析listen 的ip和端口
	Token::TokenMap::iterator itCur = configMap.find("listen");
	if(itCur == configMap.end()){
		return false;
	}
	parse_ip_port(&m_listenInfo, itCur->second);
	// 解析连接的目的地址
	std::string mark = "des";
	for(auto &kv : configMap){
		if(Token::startWith(kv.first, mark)){
			SocketInformation info;
			parse_ip_port(&info, kv.second);
			m_destinations.push_back(info);
		}
	}
	fprintf(stderr, "listen ip=%s port=%d des size=%d\n", m_listenInfo.ip, m_listenInfo.port, (int)m_destinations.size());
	return true;
}

NS_HIVE_END
