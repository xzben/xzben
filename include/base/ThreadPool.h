/************************************************************************************
 *	File   Name: ThreadPool.h														*
 * 	Description: 线程池类，既包含指定数量线程的线程集合类							* 													
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_17_THREAD_POOL_H__
#define __2013_08_17_THREAD_POOL_H__

#include "Thread.h"
#include "MutexQueue.h"

namespace XZBEN
{
class ThreadPool
{
public:
	typedef	MutexQueue<ThreadTask>			MutexTaskQueue;
	typedef std::vector<Thread*>			ThreadQueue;
	ThreadPool();
	virtual ~ThreadPool();
	bool Run(int nThreadNum);
	bool Accept(MethodPointer pfnMethod, void *pObj, void* pParam);
	bool Accept(pThreadFunc	pfnFunc, void *pParam);
	bool Run(int nThreadNum, MethodPointer pfnMethod, void *pObj, void* pParam);
	bool Run(int nThreadNum, pThreadFunc pfnFunc, void *pParam);
	void WaitStop(unsigned long lMillSecond = INFINITE);
private:
	static void __stdcall TMain(void *pParam);
	bool CreateThread(int nThreadNUm, ThreadTask task);
	void ClearThreadPool();
private:
	MutexTaskQueue			m_mutexTaskQueue;
	ThreadQueue				m_vThreads;
	bool					m_bRun;
};

};//namespace XZBEN
#endif//__2013_08_17_THREAD_POOL_H__