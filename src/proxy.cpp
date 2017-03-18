//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/18
// Time: 下午6:33
// To change this template use File | Settings | File Templates.
//

#include "proxy.h"

NS_HIVE_BEGIN

Proxy::Proxy(void) : RefObject() {

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

NS_HIVE_END
