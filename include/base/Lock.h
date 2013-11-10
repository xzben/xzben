/************************************************************************************
 *	File   Name: Lock.h																*
 * 	Description: 此文件中主要提供了两个一个用于提供互斥保护的类Mutex				*
 *				 与一个自动锁(AutoLock)												* 														
 *																					*
 *	Create Time: 07/28/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_07_28_LOCK_H__
#define __2013_07_28_LOCK_H__
#include "Base_system.h"

namespace XZBEN
{
class Mutex
{
public:
	Mutex();
	virtual ~Mutex();
	void	Lock();
	void	Unlock();
private:
	CRITICAL_SECTION m_csLock;
};

class AutoLock
{
public:
	AutoLock(Mutex* lock);
	virtual ~AutoLock();
	void Unlock();
	void Lock();
private:
	Mutex*	m_pMutex;
	bool	m_bIsLock;
};


};//namespace XZBEN
#endif//__2013_07_28_LOCK_H__