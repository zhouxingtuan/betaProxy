//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午2:58
// To change this template use File | Settings | File Templates.
//

#include "packet.h"

NS_HIVE_BEGIN

Packet::Packet(int bufferSize) : RefObject(),m_pBuffer(NULL),m_cursor(0) {
	m_pBuffer = new Buffer(bufferSize);
	m_pBuffer->retain();
}
Packet::Packet(Buffer* pBuffer) : RefObject(),m_pBuffer(pBuffer),m_cursor(0) {
	pBuffer->retain();
}
Packet::~Packet(void){
	SAFE_RELEASE(m_pBuffer)
}


NS_HIVE_END
