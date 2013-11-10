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
public:
	SharePtr(void* pObj = NULL)
	{
		if(NULL == pObj) 
		{
			m_pnUseCount = NULL;
			m_bReleased = true;
		}
		else
		{
			m_pnUseCount = new int(1);
			m_bReleased = false;
		}
		m_pObj = static_cast<Obj*>(pObj);
	}
	SharePtr(const SharePtr& rSharePtr)
	{
		if(rSharePtr != NULL)
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
			m_pnUseCount = NULL;
		}
		m_pObj = NULL;
		m_bReleased = true;
	}
	~SharePtr()
	{
		Release();
	}

	SharePtr<Obj>& operator=(const SharePtr& sharePtr)
	{
		if(*this == sharePtr) return *this;
		Release();
		if(sharePtr != NULL)
			Atom::AtomAdd(sharePtr.m_pnUseCount, 1);

		m_pnUseCount = sharePtr.m_pnUseCount;
		m_pObj = sharePtr.m_pObj;
		m_bReleased = sharePtr.m_bReleased;
		return *this;
	}

	SharePtr<Obj>& operator=(void *pObj)
	{
		if(*this == pObj) return *this;
		Release();
		if(NULL == pObj)
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
		assert(NULL != m_pObj);
		return m_pObj;
	}

	const Obj* operator->()const
	{
		assert(NULL != m_pObj);
		return m_pObj;
	}

	Obj& operator*()
	{
		assert(NULL != m_pObj);
		return *m_pObj;
	}

	const Obj& operator*()const
	{
		assert(NULL != m_pObj);
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