//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/5/28
// Time: 下午2:57
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__packet__
#define __hive__packet__

#include "buffer.h"

NS_HIVE_BEGIN

class Packet : public RefObject
{
protected:
	Buffer* m_pBuffer;		// 数据指针
	int m_cursor;			// 数据读取的偏移
public:
    explicit Packet(int bufferSize);
    explicit Packet(Buffer* pBuffer);
    virtual ~Packet(void);

	inline void setCursorToEnd(void){ m_cursor = getLength(); }
	inline void setCursor(int cur){ m_cursor = cur; }
	inline void moveCursor(int length){ m_cursor += length; }
	inline int getCursor(void) const { return m_cursor; }
	inline void resetCursor(void){ m_cursor = 0; }
	inline Buffer* getBuffer(void){ return m_pBuffer; }
	inline char* getOffsetPtr(int offset){ return m_pBuffer->data() + offset; }
	inline char* getCursorPtr(void){ return m_pBuffer->data() + m_cursor; }
	inline char* getDataPtr(void){ return m_pBuffer->data(); }
	inline int getLength(void) const { return (int)m_pBuffer->size(); }
	inline bool isCursorEnd(void) const { return getCursor() >= getLength(); }

	inline int write(const void* ptr, int length){
		int n = m_pBuffer->write(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}
	inline int read(void* ptr, int length){
		int n = m_pBuffer->read(ptr, length, getCursor());
		if( n > 0 ){
			moveCursor(n);
		}
		return n;
	}
};// end class Packet

NS_HIVE_END

#endif
