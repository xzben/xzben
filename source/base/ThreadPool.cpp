/************************************************************************************
 *	File   Name:																	*
 * 	Description:																	* 													
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#include "ThreadPool.h"

namespace XZBEN
{
ThreadPool::ThreadPool()
{
	m_bRun = false;
	m_vThreads.clear();
}

ThreadPool::~ThreadPool()
{
	ClearThreadPool();
}

void ThreadPool::ClearThreadPool()
{
	WaitStop(0);
	m_mutexTaskQueue.clear();
}

bool ThreadPool::Accept(MethodPointer pfnMethod, void *pObj, void* pParam)
{
	ThreadTask task;
	if( !task.Accept(pfnMethod, pObj, pParam) ) return false;
	m_mutexTaskQueue.push(task);
	return true;
}

bool ThreadPool::Accept(pThreadFunc	pfnFunc, void *pParam)
{
	ThreadTask task;
	if( !task.Accept(pfnFunc, pParam) ) return false;
	m_mutexTaskQueue.push(task);

	return true;
}

bool ThreadPool::Run(int nThreadNum)
{
	ThreadTask task;
	task.Accept(TMain, this);
	return CreateThread(nThreadNum, task);
}
bool ThreadPool::Run(int nThreadNum, MethodPointer pfnMethod, void *pObj, void* pParam)
{
	ThreadTask task;
	task.Accept(pfnMethod, pObj, pParam);
	return CreateThread(nThreadNum, task);
}
bool ThreadPool::Run(int nThreadNum, pThreadFunc pfnFunc, void *pParam)
{
	ThreadTask task;
	task.Accept(pfnFunc, pParam);
	return CreateThread(nThreadNum, task);
}
bool ThreadPool::CreateThread(int nThreadNum, ThreadTask task)
{
	m_bRun = true;
	for(int i = 0; i < nThreadNum; i++)
	{
		Thread* pThread = new Thread;
		pThread->Run(task);
		m_vThreads.push_back(pThread);
	}
	return true;
}
 
void __stdcall ThreadPool::TMain(void *pParam)
{
	ThreadPool *pThreadPool = (ThreadPool*)pParam;
	while(pThreadPool->m_bRun)
	{
		ThreadTask task;
		if( pThreadPool->m_mutexTaskQueue.front_pop(task) )
		{
			task.Run();
		}
		else
		{
			Sleep(10);
		}
	}
}

void ThreadPool::WaitStop(unsigned long lMillSecond /*= INFINITE*/)
{
	if(lMillSecond != INFINITE)
		m_bRun = false;

	for(int i = 0; i < (int)m_vThreads.size(); i++)
	{
		Thread* pThread = m_vThreads[i];
		pThread->WaitStop(lMillSecond);
		SAFE_DELETE(pThread);
	}
	m_vThreads.clear();
}

};//namespace XZBEN