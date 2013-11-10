/************************************************************************************
 *	File   Name:																	*
 * 	Description: 																	* 														
 *																					*
 *	Create Time: mm/dd/YYYY															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/
#include "Lock.h"

namespace XZBEN
{
//////////////////////////////////////////////////////////////////////////////////////////////////
//Mutex Class public Member
Mutex::Mutex()
{
	InitializeCriticalSection( &m_csLock );
}

Mutex::~Mutex()
{
	DeleteCriticalSection( &m_csLock );
}

void Mutex::Lock()
{
	EnterCriticalSection( &m_csLock );
}

void Mutex::Unlock()
{
	LeaveCriticalSection( &m_csLock );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//AutoLock Class public Member
AutoLock::AutoLock(Mutex* lock)
	:m_pMutex(lock), m_bIsLock(false)
{
	Lock();
}

AutoLock::~AutoLock()
{
	Unlock();
}

void AutoLock::Unlock()
{
	if(m_bIsLock)
	{
		m_pMutex->Unlock();
		m_bIsLock = false;
	}
}
void AutoLock::Lock()
{
	if( !m_bIsLock )
	{
		m_pMutex->Lock();
		m_bIsLock = true;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////
};//namespace XZBEN

