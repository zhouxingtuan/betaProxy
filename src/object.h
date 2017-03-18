//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/9/4
// Time: 下午12:49
// To change this template use File | Settings | File Templates.
//

#ifndef __hive__object__
#define __hive__object__

#include "common.h"

NS_HIVE_BEGIN

#define INVALID_UNIQUE_HANDLE 0

#define MAX_1616_HASH_SIZE 65535
#define MAX_0824_HASH_SIZE 16777215

class Object1616
{
public:
	typedef uint32 handle_type;
	typedef uint32 version_type;
	typedef uint32 index_type;
	typedef struct Handle{
		union{
			struct{
				version_type 	version 	: 16;	// 版本
				index_type 		index 		: 16;	// 下标
			};
			handle_type 		handle 		: 32;	// 句柄
		};
	public:
		Handle(handle_type h){ this->handle = h; }
		Handle(void) : handle(0) {}
		~Handle(void){}

		inline index_type getIndex(void) const { return this->index; }
		inline version_type getVersion(void) const { return this->version; }
		inline handle_type getHandle(void) const { return this->handle; }
		inline void setIndex(index_type index){ this->index = index; }
		inline void increaseVersion(void){ ++(this->version); }
		inline bool operator==(handle_type h) const { return (this->handle == h); }
		inline bool operator==(const Handle& h) const{ return (this->handle == h.handle); }
		inline bool operator<(handle_type h) const { return (this->handle < h); }
		inline bool operator<(const Handle& h) const { return (this->handle < h.handle); }
		inline Handle& operator=(handle_type h){ this->handle = h; return *this; }
	}Handle;
public:
	Object1616(void) : m_handle(0) {}
	virtual ~Object1616(void){}

	inline index_type getIndex(void) const { return m_handle.getIndex(); }
	inline version_type getVersion(void) const { return m_handle.getVersion(); }
	inline handle_type getHandle(void) const { return m_handle.getHandle(); }
	inline void setIndex(index_type index) { return m_handle.setIndex(index); }
	inline void increaseVersion(void) { m_handle.increaseVersion(); }
	inline void setHandle(handle_type handle) { m_handle = handle; }
protected:
	Handle m_handle;
};

class Object0824
{
public:
	typedef uint32 handle_type;
	typedef uint32 version_type;
	typedef uint32 index_type;
	typedef struct Handle{
		union{
			struct{
				version_type 	version 	: 8;	// 版本
				index_type 		index 		: 24;	// 下标
			};
			handle_type 		handle 		: 32;	// 句柄
		};
	public:
		Handle(handle_type h){ this->handle = h; }
		Handle(void) : handle(0) {}
		~Handle(void){}

		inline index_type getIndex(void) const { return this->index; }
		inline version_type getVersion(void) const { return this->version; }
		inline handle_type getHandle(void) const { return this->handle; }
		inline void setIndex(index_type index){ this->index = index; }
		inline void increaseVersion(void){ ++(this->version); }
		inline bool operator==(handle_type h) const { return (this->handle == h); }
		inline bool operator==(const Handle& h) const{ return (this->handle == h.handle); }
		inline bool operator<(handle_type h) const { return (this->handle < h); }
		inline bool operator<(const Handle& h) const { return (this->handle < h.handle); }
		inline Handle& operator=(handle_type h){ this->handle = h; return *this; }
	}Handle;
public:
	Object0824(void) : m_handle(0) {}
	virtual ~Object0824(void){}

	inline index_type getIndex(void) const { return m_handle.getIndex(); }
	inline version_type getVersion(void) const { return m_handle.getVersion(); }
	inline handle_type getHandle(void) const { return m_handle.getHandle(); }
	inline void setIndex(index_type index) { return m_handle.setIndex(index); }
	inline void increaseVersion(void) { m_handle.increaseVersion(); }
	inline void setHandle(handle_type handle) { m_handle = handle; }
protected:
	Handle m_handle;
};

#define DEFAULT_MAX_OBJECT_POOL MAX_1616_HASH_SIZE

template<class _OBJECT_>
class ObjectPool : public RefObject
{
public:
	typedef std::vector<void*> ObjectVector;
	typedef std::vector<typename _OBJECT_::handle_type> HandleVector;
public:
	ObjectPool(void) : RefObject(), m_maxHashNumber(DEFAULT_MAX_OBJECT_POOL) {
		m_objects.resize(1, NULL);	// 下标为0的值不使用
	}
	virtual ~ObjectPool(void){
		clear();
	}

	// 当超出最大缓存数值的时候，会返回NULL
	_OBJECT_* create(void){
		typename _OBJECT_::index_type index;
		struct _OBJECT_::Handle h;
		_OBJECT_* pObj;
		if( !m_idleHandle.empty() ){
			h = m_idleHandle.back();
			m_idleHandle.pop_back();
			index = h.getIndex();
			pObj = (_OBJECT_*)m_objects[index];
			if( NULL == pObj ){
				pObj = _OBJECT_::createObject();
				pObj->setHandle(h.getHandle());
				m_objects[index] = pObj;
			}
		}else{
			index = (typename _OBJECT_::index_type)m_objects.size();
			if(index > m_maxHashNumber){
				return NULL;
			}
			pObj = _OBJECT_::createObject();
			pObj->setIndex(index);
			m_objects.push_back(pObj);
		}
		return pObj;
	}
	_OBJECT_* get(typename _OBJECT_::handle_type handle){
		_OBJECT_* pObj = NULL;
		struct _OBJECT_::Handle h(handle);
		typename _OBJECT_::index_type index = h.getIndex();
		if( index < (typename _OBJECT_::index_type)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			return pObj;
		}
		return NULL;
	}
	bool idle(_OBJECT_* pObj){
		return idle(pObj->getHandle());
	}
	bool idle(typename _OBJECT_::handle_type handle){
		_OBJECT_* pObj = NULL;
		struct _OBJECT_::Handle h(handle);
		typename _OBJECT_::index_type index = h.getIndex();
		if( index < (typename _OBJECT_::index_type)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			pObj->increaseVersion();
			m_idleHandle.push_back(pObj->getHandle());
			return true;
		}
		return false;
	}
	bool remove(_OBJECT_* pObj){
		return remove(pObj->getHandle());
	}
	bool remove(typename _OBJECT_::handle_type handle){
		_OBJECT_* pObj = NULL;
		struct _OBJECT_::Handle h(handle);
		typename _OBJECT_::index_type index = h.getIndex();
		if( index < (typename _OBJECT_::index_type)m_objects.size() ){
			pObj = (_OBJECT_*)m_objects[index];
		}
		if( NULL != pObj && pObj->getHandle() == handle ){
			m_objects[index] = NULL;
			_OBJECT_::releaseObject(pObj);
			h.increaseVersion();
			m_idleHandle.push_back(h.getHandle());
			return true;
		}
		return false;
	}
	void clear(void){
		for(auto pObj : m_objects){
			_OBJECT_::releaseObject((_OBJECT_*)pObj);
		}
		m_objects.clear();
		m_objects.resize(1, NULL);	// 下标为0的值不使用
		m_idleHandle.clear();
	}
	uint32 size(void) const { return (uint32)m_objects.size(); }
	uint32 getMaxHashNumber(void) const { return m_maxHashNumber; }
	void setMaxHashNumber(uint32 number) { m_maxHashNumber = number; }
	uint32 getIdleSize(void) const { return (uint32)m_idleHandle.size(); }
	ObjectVector& getObjectVector(void) { return m_objects; }
protected:
	ObjectVector m_objects;
	HandleVector m_idleHandle;
	uint32 m_maxHashNumber;
};

NS_HIVE_END

#endif
