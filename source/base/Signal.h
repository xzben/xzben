/************************************************************************************
 *	File   Name: Signal.h															*
 * 	Description: 提供一个信号量类，实现多线程的同步控制								* 														
 *																					*
 *	Create Time: 07/28/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_07_28_EVENT_H__
#define __2013_07_28_EVENT_H__
#include "Base_system.h"

namespace XZBEN
{
class Signal 
{
public:
	Signal(bool bManualReset = false, bool bIsSignal = false, 
		const char* lpszEventName = NULL);
	Signal(HANDLE	hEvent);
	virtual ~Signal();
public:
	virtual bool	Close();
	virtual	void	Notify();
	virtual bool	SetSignalStatue(bool bIsSignal = true);
	virtual bool	Wait(unsigned long ulMillsecond = INFINITE);
	virtual int		WaitMutilObject(int nSignalNum, Signal *sgArr[], bool bWaitAll,unsigned long ulMillsecond = INFINITE);
	virtual HANDLE	GetSignalObject();
private:
	HANDLE	m_hEvent;
};

};//namespace XZBEN
#endif//__2013_07_28_EVENT_H__