//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/7/9
// Time: 上午11:39
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__listener__
#define __hive__listener__

#include "epoll.h"
#include "object.h"

NS_HIVE_BEGIN


#define MAX_LISTEN_SIZE 10000

class Listener;

typedef void (*AcceptSocketFunction)(int fd, const char* ip, uint16 port, Listener* pListener);

class Listener : public EpollObject, public Object1616
{
public:
	Listener(void);
	virtual ~Listener(void);

	static Listener* createObject(void){
		return new Listener();
	}
	static void releaseObject(Listener* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	// from EpollObject
	virtual bool epollActive(uint32 events);
	virtual bool epollIn(void){ return true; }
	virtual bool epollOut(void){ return true; }
	virtual void epollRemove(void);
	virtual void epollCheck(void);

	bool openSocket(void);
	void setAcceptSocketFunction(AcceptSocketFunction pFunc){ m_acceptSocketFunction = pFunc; }
	AcceptSocketFunction getAcceptSocketFunction(void){ return m_acceptSocketFunction; }
	void resetData(void);
protected:
	AcceptSocketFunction m_acceptSocketFunction;
};

NS_HIVE_END

#endif
