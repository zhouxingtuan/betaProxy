//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/26
// Time: 上午8:02
// To change this template use File | Settings | File Templates.
//

#include "proxy.h"

USING_NS_HIVE;

class MyProxyLogic : HiveNS::ProxyLogic
{
public:
	MyProxyLogic(void) : HiveNS::ProxyLogic(){}
	virtual ~MyProxyLogic(void){}

	// receive connection called
	virtual void onReceiveAccept(uint32 handle, const char* ip, uint16 port){
		// open the partner ?

	}
	// receive message from accept;
	// after client to back server create success this will never call again;
	virtual void onReceiveMessage(uint32 handle, Buffer* pBuffer){
		// open the partner ? check the first message from front direction ?
		SocketInformation* pInfo = Proxy::getInstance()->getNextDestination();
		if(pInfo == NULL){
			fprintf(stderr, "can not find a destination in proxy config\n");
			Proxy::getInstance()->closeAccept(handle);
			return;
		}
		uint32 clientHandle = Proxy::getInstance()->openPartner(handle, pInfo->ip, pInfo->port);
		if(clientHandle == 0){
			fprintf(stderr, "openPartner client failed ip=%s, port=%d\n", pInfo->ip, pInfo->port);
			Proxy::getInstance()->closeAccept(handle);
			return;
		}
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
	fprintf(stderr, "proxy main exit\n");
	Proxy::destroyInstance();

	return 0;
}