/************************************************************************************
 *	File   Name: SharePtr.h															*
 * 	Description: 智能指针类，会根据指向对象的引用计数在适当的时候释放对象内存		* 														
 *																					*
 *	Create Time: 08/04/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_04_SHARE_POINTER_H__
#define __2013_08_04_SHARE_POINTER_H__

#include "Base_system.h"
#include "atom.h"

namespace XZBEN
{
template<typename Obj>
class SharePtr
{
public:
	template<typename Obj>
	friend bool operator==(void *pObj, const SharePtr<Obj>& rSharePtr);
	template<typename Obj>
	friend bool operator!=(void *pObj, const SharePtr<Obj>& rSharePtr);
	
	template<typename Obj>
	friend bool operator==(const SharePtr<Obj>& rSharePtr, void *pObj);
	template<typename Obj>
	friend bool operator!=(const SharePtr<Obj>& rSharePtr, void *pObj);

	friend class SharePtr;
public:
	SharePtr(void* pObj = nullptr)
	{
		if(nullptr == pObj) 
		{
			m_pnUseCount = nullptr;
			m_bReleased = true;
		}
		else
		{
			m_pnUseCount = new int(1);
			m_bReleased = false;
		}
		m_pObj = static_cast<Obj*>(pObj);
	}
	template<typename ObjTwo>
	SharePtr(const SharePtr<ObjTwo>& rsharePtr)
	{
		if(rsharePtr != nullptr)
			Atom::AtomAdd(rsharePtr.m_pnUseCount, 1);

		m_pnUseCount = rsharePtr.m_pnUseCount;
		m_pObj = rsharePtr.m_pObj;
		m_bReleased = rsharePtr.m_bReleased;
	}

	SharePtr(const SharePtr& rSharePtr)
	{
		if(rSharePtr != nullptr)
			Atom::AtomAdd(rSharePtr.m_pnUseCount, 1);

		m_pnUseCount = rSharePtr.m_pnUseCount;
		m_pObj = rSharePtr.m_pObj;
		m_bReleased = rSharePtr.m_bReleased;
	}
	void Release()
	{
		if(m_bReleased) return;

		if(1 == Atom::AtomDec(m_pnUseCount, 1))
		{
			delete m_pObj;
			delete m_pnUseCount;
			m_pnUseCount = nullptr;
		}
		m_pObj = nullptr;
		m_bReleased = true;
	}
	~SharePtr()
	{
		Release();
	}
	template<typename ObjOne, typename ObjTwo>
	SharePtr<ObjOne>& operator=(const SharePtr<ObjTwo>& rsharePtr)
	{
		if(*this == rsharePtr) return *this;
		Release();
		if(sharePtr != nullptr)
			Atom::AtomAdd(sharePtr.m_pnUseCount, 1);

		m_pnUseCount = sharePtr.m_pnUseCount;
		m_pObj = sharePtr.m_pObj;
		m_bReleased = sharePtr.m_bReleased;
		return *this;
	}
	SharePtr<Obj>& operator=(const SharePtr& rsharePtr)
	{
		if(*this == rsharePtr) return *this;
		Release();
		if(rsharePtr != nullptr)
			Atom::AtomAdd(rsharePtr.m_pnUseCount, 1);

		m_pnUseCount = rsharePtr.m_pnUseCount;
		m_pObj = rsharePtr.m_pObj;
		m_bReleased = rsharePtr.m_bReleased;
		return *this;
	}

	SharePtr<Obj>& operator=(void *pObj)
	{
		if(*this == pObj) return *this;
		Release();
		if(nullptr == pObj)
		{
			return *this;
		}
		m_pnUseCount = new int(1);
		m_pObj = static_cast<Obj*>(pObj);
		m_bReleased = false;

		return *this;
	}

	Obj* operator->()
	{
		assert(nullptr != m_pObj);
		return m_pObj;
	}

	const Obj* operator->()const
	{
		assert(nullptr != m_pObj);
		return m_pObj;
	}

	Obj& operator*()
	{
		assert(nullptr != m_pObj);
		return *m_pObj;
	}

	const Obj& operator*()const
	{
		assert(nullptr != m_pObj);
		return *m_pObj;
	}
	bool operator==(const SharePtr<Obj> &sharePtr)const
	{
		return (m_pObj == sharePtr.m_pObj);
	}

	bool operator!=(const SharePtr<Obj> &sharePtr)const
	{
		return (m_pObj != sharePtr.m_pObj);
	}
	const Obj* GetObj()const
	{
		assert(nullptr != m_pObj);
		return m_pObj;
	}
	Obj*	GetObj()
	{
		assert(nullptr != m_pObj);
		return m_pObj;
	}
private:
	int		*m_pnUseCount;
	Obj		*m_pObj;
	bool	m_bReleased;
};

template<typename Obj>
bool operator==(void *pObj, const SharePtr<Obj>& rSharePtr)
{
	return (pObj == rSharePtr.m_pObj);
}

template<typename Obj>
bool operator!=(void *pObj, const SharePtr<Obj>& rSharePtr)
{
	return (pObj != rSharePtr.m_pObj);
}

template<typename Obj>
bool operator==(const SharePtr<Obj>& rSharePtr, void *pObj)
{
	return (pObj == rSharePtr.m_pObj);
}

template<typename Obj>
bool operator!=(const SharePtr<Obj>& rSharePtr, void *pObj)
{
	return (pObj != rSharePtr.m_pObj);
}

};//namespace XZBEN
#endif//__2013_08_04_SHARE_POINTER_H__