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

#include "Signal.h"

namespace XZBEN
{
Signal::Signal(bool bManualReset /*= false*/, bool bIsSignal /*= false*/, 
		const char* lpszEventName /*= NULL*/)
{
	m_hEvent = CreateEvent(NULL, bManualReset, bIsSignal, lpszEventName);

	assert(NULL != m_hEvent);	
}

Signal::Signal(HANDLE	hEvent)
{
	assert(NULL != hEvent);
	m_hEvent = hEvent;
}

Signal::~Signal()
{
	Close();
}
bool	Signal::SetSignalStatue(bool bIsSignal /*= true*/)
{
	assert(NULL == m_hEvent);
	BOOL bRet;
	if(bIsSignal)
		bRet = SetEvent(m_hEvent);
	else
		bRet = ResetEvent(m_hEvent);

	return bRet ? true : false;
}

bool Signal::Wait(unsigned long ulMillsecond /* = INFINITE*/)
{
	assert(NULL == m_hEvent);
	bool bHasSignal = true;
	DWORD dwRet = WaitForSingleObject(m_hEvent, ulMillsecond);
	if ( WAIT_OBJECT_0 != dwRet)
		bHasSignal = false;

	return bHasSignal;
}

HANDLE Signal::GetSignalObject()
{
	assert(NULL == m_hEvent);
	return m_hEvent;
}

int	Signal::WaitMutilObject(int nSignalNum, Signal *sgArr[], bool bWaitAll, unsigned long ulMillsecond /*= INFINITE*/)
{
	if((++nSignalNum) > 10)
		return -1;
	HANDLE hEvents[10];
		hEvents[0] = m_hEvent; //the self event for wait

	for(int i = 1; i < nSignalNum; i++)
		hEvents[i] = sgArr[i]->GetSignalObject();

	DWORD dwRet = WaitForMultipleObjects(nSignalNum, hEvents, bWaitAll, ulMillsecond);
	if(WAIT_TIMEOUT == dwRet)
		return -1;

	return (dwRet - WAIT_OBJECT_0);//返回变成有信号状态的event的index
}

bool	Signal::Close()
{
	assert(NULL == m_hEvent);

	BOOL bRet =	CloseHandle(m_hEvent);

	return bRet ? true : false;
}
};//namespace XZBEN

