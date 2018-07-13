//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/26
// Time: 上午8:02
// To change this template use File | Settings | File Templates.
//

#include "proxy.h"

USING_NS_HIVE;

class MyProxyLogic : public HiveNS::ProxyLogic
{
public:
	MyProxyLogic(void) : HiveNS::ProxyLogic(){}
	virtual ~MyProxyLogic(void){}

	// receive connection called
	virtual void onReceiveAccept(uint32 handle, const char* ip, uint16 port){
		// open the partner ?

        // todo : Use this codes to open connect right after receive a new connection

		// check if the partner is opened
		if(Proxy::getInstance()->isPartnerOpened(handle)){
			LOG_DEBUG("partner is already opened handle=%d", handle);
			return;
		}

		SocketInformation* pInfo = Proxy::getInstance()->getNextDestination();
		if(pInfo == NULL){
			LOG_ERROR("can not find a destination in proxy config");
			Proxy::getInstance()->closeAccept(handle);
			return;
		}
		uint32 clientHandle = Proxy::getInstance()->openPartner(handle, pInfo->ip, pInfo->port);
		if(clientHandle == 0){
			LOG_ERROR("openPartner client failed ip=%s, port=%d", pInfo->ip, pInfo->port);
			Proxy::getInstance()->closeAccept(handle);
			return;
		}
	}
	// receive message from accept;
	// after client to back server create success this will never call again;
	virtual void onReceiveMessage(uint32 handle, Buffer* pBuffer, const char* ip, uint16 port){
		// open the partner ? check the first message from front direction ?

        // todo : use this codes if you want to identify the connection.

//		// check if the partner is opened
//		if(Proxy::getInstance()->isPartnerOpened(handle)){
//			LOG_DEBUG("partner is already opened handle=%d", handle);
//			return;
//		}
//
//		SocketInformation* pInfo = Proxy::getInstance()->getNextDestination();
//		if(pInfo == NULL){
//			LOG_ERROR("can not find a destination in proxy config");
//			Proxy::getInstance()->closeAccept(handle);
//			return;
//		}
//		uint32 clientHandle = Proxy::getInstance()->openPartner(handle, pInfo->ip, pInfo->port);
//		if(clientHandle == 0){
//			LOG_ERROR("openPartner client failed ip=%s, port=%d", pInfo->ip, pInfo->port);
//			Proxy::getInstance()->closeAccept(handle);
//			return;
//		}
	}
};

int main(int argc, char *argv[])
{
	MyProxyLogic myLogic;
	// 使用默认的信号处理
	defaultSignalHandle();

	Proxy::createInstance();
	Proxy::getInstance()->setProxyLogic(&myLogic);
	Proxy::getInstance()->initialize();
	Proxy::getInstance()->update();
	LOG_INFO("proxy main exit");
	Proxy::destroyInstance();

	return 0;
}