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
	m_pListenerPool(NULL), m_pAcceptPool(NULL), m_pClientPool(NULL) {

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
		m_pEpoll->setWorker(this);	// 设置Worker指针
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
}
void Proxy::destroy(void){
	SAFE_RELEASE(m_pEpoll)
	SAFE_RELEASE(m_pTimer)
	SAFE_RELEASE(m_pListenerPool)
	SAFE_RELEASE(m_pAcceptPool)
	SAFE_RELEASE(m_pClientPool)
}

NS_HIVE_END
