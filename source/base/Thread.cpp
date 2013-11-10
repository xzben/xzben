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

#include"Thread.h"

namespace XZBEN
{
/////////////////////////////////////////////////////////////////////////////////////////
//ThreadTask class public method
ThreadTask::ThreadTask()
{
	Reset();
}

ThreadTask::~ThreadTask()
{
		
}

bool ThreadTask::Accept(MethodPointer pfnMethod, void *pObj, void* pParam)
{
	Reset();
	if(0 == pfnMethod || 0 == pObj)
	{
		return false;
	}
	m_pfnMethodFunc = pfnMethod;
	m_pParam = pParam;
	m_pObj = pObj;
	return true;
}

bool ThreadTask::Accept(pThreadFunc	pfnFunc, void *pParam)
{
	Reset();
	if(0 == pfnFunc)
	{
		return false;
	}
	m_pfnFunc = pfnFunc;
	m_pParam = pParam;
	return true;
}

bool ThreadTask::Run()
{
	if(0 != m_pObj && 0 != m_pfnMethodFunc)
	{
		pThreadMemberFunc method = (pThreadMemberFunc)m_pfnMethodFunc;
		method(m_pObj, m_pParam);
		return true;
	}
	else if(0 != m_pfnFunc)
	{
		m_pfnFunc(m_pParam);
		return true;
	}
	return false;
}
void ThreadTask::Reset()
{
	m_pfnMethodFunc = 0;
	m_pfnFunc = 0;
	m_pParam = 0;
	m_pObj = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//Thread Class public Member
Thread::Thread()
{
	m_tsTask.Reset();
	m_bIsRun = false;
	m_hThread = NULL;
	m_dwThreadID = 0;
}

Thread::~Thread()
{
	WaitStop(0); //立即结束线程
}

DWORD* Thread::GetThreadID()
{
	return &m_dwThreadID;
}

HANDLE	Thread::GetHanle()
{
	return m_hThread;
}

bool Thread::Run(MethodPointer pfnMethod, void *pObj, void* pParam)
{
	if(!m_tsTask.Accept(pfnMethod, pObj, pParam))
	{
		return false;
	}
	return Run();
}
bool Thread::Run(pThreadFunc pfnFunc, void *pParam)
{
	if(!m_tsTask.Accept(pfnFunc, pParam))
	{
		return false;
	}
	return Run();
}
bool Thread::Run(ThreadTask task)
{
	m_tsTask = task;
	return Run();
}

bool Thread::Run()
{
	m_hThread = CreateThread(NULL, 
							0, 
							(LPTHREAD_START_ROUTINE)TMain,
							(void*) this, 
							0, 
							&m_dwThreadID);
	if(NULL == m_hThread)
	{
		return false;
	}
	return true;
}

void Thread::WaitStop(unsigned long lMillSecond /*= INFINITE*/)
{
	if(NULL == m_hThread) 
		return ;
	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hThread, lMillSecond))
	{
		TerminateThread( m_hThread, 0 );//强制终止线程
	}
	CloseHandle(m_hThread);
	m_hThread = NULL;
}

DWORD __stdcall Thread::TMain(void *pParam)
{
	Thread *pThread = (Thread*)pParam;
	pThread->Main();

	return 0;
}

void Thread::Main()
{
	m_bIsRun = true;
	m_tsTask.Run();
	m_bIsRun = false;
}

bool Thread::IsRuning()
{
	return m_bIsRun;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
};//namespace XZBEN

