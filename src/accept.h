//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:38
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__accept__
#define __hive__accept__

#include "epoll.h"
#include "object.h"
#include "packet.h"
#include "timer.h"

NS_HIVE_BEGIN

#define MAX_LENGTH_NOT_IDENTIFY 128
#define MAX_LENGTH_IDENTIFY 4194312	// 4M + 8

enum ConnectionState {
	CS_DISCONNECT = 0,
	CS_CONNECT_START,
	CS_CONNECT_OK,
};

class Accept;

typedef int64 (*ConnectTimeoutCallback)(Accept* pAccept);

class Accept : public EpollObject, public Object1616, public TimerObject
{
public:
	typedef std::deque<Packet*> PacketQueue;
protected:
	ConnectTimeoutCallback m_timerCallback;	// 回调函数
	PacketQueue m_packetQueue;
	Packet* m_tempReadPacket;
	Accept* m_pPartner;
	unsigned char m_connectionState;
public:
	explicit Accept(void);
	virtual ~Accept(void);

	static Accept* createObject(void){
		return new Accept();
	}
	static void releaseObject(Accept* pObj){
		if(NULL != pObj){
			delete pObj;
		}
	}

	// from EpollObject
	virtual bool epollActive(uint32 events){ return false; }
	virtual void epollIn(void);
	virtual void epollOut(void);
	virtual void epollRemove(void);
	virtual void epollCheck(void);

	// from TimerObject
	virtual int64 timerCallback(void);

	void closePartner(void);
	void sendHashBufferToPartner(void);

	bool setTimeout(int64 timeCount, ConnectTimeoutCallback callback);
	inline void setPartner(Accept* pAccept){ m_pPartner = pAccept; }
	inline Accept* getPartner(void){ return m_pPartner; }
	bool sendPacket(Packet* pPacket);
	inline void setConnectionState(unsigned char state) { m_connectionState = state; }
	inline unsigned char getConnectionState(void) const { return (unsigned char)m_connectionState; }
	inline bool isIdentify(void) const { return (m_connectionState >= CS_CONNECT_OK); }
	inline int getMaxLength(void) const {
		if(isIdentify()){
			return MAX_LENGTH_IDENTIFY;
		}else{
			return MAX_LENGTH_NOT_IDENTIFY;
		}
	}
	void resetData(void);
protected:
	int readSocket(void);
	int writeSocket(Packet* pPacket);
	void dispatchPacket(Packet* pPacket);
	void releasePacket(void);
};//end class Accept

NS_HIVE_END

#endif
