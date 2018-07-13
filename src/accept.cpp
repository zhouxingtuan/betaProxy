//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/31
// Time: 下午9:40
// To change this template use File | Settings | File Templates.
//

#include "accept.h"
#include "proxy.h"

NS_HIVE_BEGIN

Accept::Accept(void) : EpollObject(), Object1616(), TimerObject(),
 	m_timerCallback(NULL), m_pBuffer(NULL), m_pPartner(NULL),
 	m_connectionState(CS_DISCONNECT) {

}
Accept::~Accept(void){
	releasePacket();
}
bool Accept::epollIn(void){
	int result;
	do{
		result = readSocket();
	}while(result == 0);
	if( result < 0 ){
		epollRemove();
		return false;
	}
	return true;
}
bool Accept::epollOut(void){
	Packet* pPacket;
	int result;
	do{
		pPacket = NULL;
		if( !m_packetQueue.empty() ){
			pPacket = m_packetQueue.front();
			m_packetQueue.pop_front();
		}
		if( NULL == pPacket ){
			//getEpoll()->objectChange(this, EPOLLIN);
			return true;
		}
		result = writeSocket(pPacket);
		if( result < 0 ){
			pPacket->release();		// 释放
			epollRemove();
			return false;
		}
		// result == 0 成功写 || result > 0 需要重新尝试写
		if(pPacket->isCursorEnd()){
			pPacket->release();		// 释放
		}else{
			// 没写完就说明写入buffer已经满了，等待下一次写操作
			m_packetQueue.push_front(pPacket);
			return true;
		}
	}while(1);
	return true;
}
void Accept::epollRemove(void){
	Proxy::getInstance()->closeAccept(this->getHandle());
}
void Accept::epollCheck(void){
	if( m_packetQueue.empty() ){
		getEpoll()->objectChange(this, EPOLLIN);
	}else{
		getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
	}
}

int64 Accept::timerCallback(void){
	if(NULL != m_timerCallback){
		return m_timerCallback(this);
	}
	return -1;
}
bool Accept::setTimeout(int64 timeCount, ConnectTimeoutCallback callback){
	m_timerCallback = callback;
	return setTimer(timeCount, NULL);
}
void Accept::closePartner(void){
	if(NULL != m_pPartner){
		m_pPartner->setPartner(NULL);
		m_pPartner->epollRemove();
		m_pPartner = NULL;
	}
}
void Accept::sendHashBufferToPartner(void){
	if(NULL == m_pBuffer || NULL == m_pPartner){
		return;
	}
	Packet* pPacket = new Packet(m_pBuffer);
	pPacket->resetCursor();		// 后面的写操作需要重置
	pPacket->retain();			// 进入队列前引用
	m_pPartner->sendPacket(pPacket);
	SAFE_RELEASE(m_pBuffer)
}
void Accept::releasePacket(void){
	for( auto pPacket : m_packetQueue ){
		pPacket->release();
	}
	m_packetQueue.clear();
}
bool Accept::sendPacket(Packet* pPacket){
	pPacket->resetCursor();		// 后面的写操作需要重置
	pPacket->retain();			// 进入队列前引用
	if( !m_packetQueue.empty() ){
		// 已经在epoll中等待out事件
		m_packetQueue.push_back(pPacket);
	}else{
		// 先执行写操作，如果出现重试，那么再将Packet入队
		int result = writeSocket(pPacket);
		if( result < 0 ){
			pPacket->release();		// 释放
			epollRemove();
		}else{
			// result == 0 成功写 || result > 0 需要重新尝试写
			if(pPacket->isCursorEnd()){
				pPacket->release();		// 释放
			}else{
				// 没有写完的消息进入队列
				m_packetQueue.push_front(pPacket);
				// 进入epoll等待
				getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
			}
		}
	}

	return true;
}
bool Accept::sendPacket(const char* ptr, int length){
	if( !m_packetQueue.empty() ){
		Packet* pPacket = new Packet(length);
		pPacket->write(ptr, length);
		pPacket->resetCursor();		// 后面的写操作需要重置
		pPacket->retain();			// 进入队列前引用
		// 已经在epoll中等待out事件
		m_packetQueue.push_back(pPacket);
	}else{
		// 先执行写操作，如果出现重试，那么再将Packet入队
		int writeLength = 0;
		int result = writeSocket(ptr, length, &writeLength);
		if( result < 0 ){
			epollRemove();
		}else{
			// result == 0 成功写 || result > 0 需要重新尝试写
			if(length == writeLength){
				// write success end, do nothing
			}else{
				int leftLength;
				if(result == 0){
					leftLength = length - writeLength;
				}else{
					leftLength = length;
				}
				Packet* pPacket = new Packet(leftLength);
				pPacket->write(ptr + writeLength, leftLength);
				pPacket->resetCursor();		// 后面的写操作需要重置
				pPacket->retain();			// 进入队列前引用
				// 没有写完的消息进入队列
				m_packetQueue.push_front(pPacket);
				// 进入epoll等待
				getEpoll()->objectChange(this, EPOLLIN | EPOLLOUT);
			}
		}
	}

	return true;
}
void Accept::resetData(void){
	closePartner();
	setConnectionState(CS_DISCONNECT);
	clearTimer();		// 停止计时器
	closeSocket();		// 关闭套接字
	releasePacket();	// 取消所有数据包的发送
	SAFE_RELEASE(m_pBuffer)
}
Buffer* Accept::getBuffer(int length){
	if(NULL == m_pBuffer){
		m_pBuffer = new Buffer(length);
		m_pBuffer->retain();
	}
	return m_pBuffer;
}
int Accept::readSocket(void){
	static char recvBuffer[8192];
    int nread;
    nread = read(this->getSocketFD(), recvBuffer, 8192);
    if(nread < 0){
        switch(errno){
        case EINTR: return 1; 	// 读数据失败，处理信号中断
        case EAGAIN: return 2;	// 可以下次重新调用
        default: return -1;
        }
        return -1;
    }else if(nread == 0){
        return -1;
    }
    // check partner
    if(NULL != m_pPartner && m_pPartner->getConnectionState() >= CS_CONNECT_OK){
		m_pPartner->sendPacket(recvBuffer, nread);
		return 0;
    }
    // hash the buffer and wait client partner to create
	Buffer* pBuffer = this->getBuffer(nread);
	pBuffer->insert(pBuffer->end(), recvBuffer, recvBuffer + nread);
	Proxy::getInstance()->getProxyLogic()->onReceiveMessage(this->getHandle(), pBuffer, this->getIP(), this->getPort());
    return 0;
}
int Accept::writeSocket(Packet* pPacket){
    int nwrite;
    nwrite = write(this->getSocketFD(), pPacket->getCursorPtr(), pPacket->getLength()-pPacket->getCursor());
    if(nwrite < 0){
        switch(errno){
        case EINTR: return 1; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
//            fprintf(stderr, "write EAGAIN capture\n");
            return 2;
        default: return -1;
        }
        return -1;
    }
    pPacket->moveCursor( nwrite );// used
    return 0;
}
int Accept::writeSocket(const char* ptr, int length, int* writeLength){
    int nwrite;
    nwrite = write(this->getSocketFD(), ptr, length);
    if(nwrite < 0){
        switch(errno){
        case EINTR: return 1; // 写数据失败，处理信号中断
        case EAGAIN:    // 可以下次重新调用
//            fprintf(stderr, "write EAGAIN capture\n");
            return 2;
        default: return -1;
        }
        return -1;
    }
    *writeLength = nwrite;
    return 0;
}

NS_HIVE_END
