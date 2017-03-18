//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 17/3/18
// Time: 下午6:33
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__proxy__
#define __hive__proxy__

#include "common.h"
#include "buffer.h"
#include "token.h"
#include "object.h"
#include "timer.h"
#include "epoll.h"
#include "accept.h"
#include "client.h"
#include "listener.h"
#include "systemsignal.h"

NS_HIVE_BEGIN

class Proxy : public RefObject
{
public:

public:
	Proxy(void);
	virtual ~Proxy(void);

	static Proxy* getInstance(void);
	static Proxy* createInstance(void);
	static void destroyInstance(void);

public:

};

NS_HIVE_END

#endif
