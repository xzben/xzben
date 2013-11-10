
/************************************************************************************
 *	File   Name: Thread.h															*
 * 	Description: 线程类，支持用类成员函数或全局函数作为线程过程函数					* 														
 *	Create Time: 07/28/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_07_28_THREAD_H__
#define __2013_07_28_THREAD_H__
#include "Base_system.h"

namespace XZBEN
{
typedef uint64	MethodPointer;
typedef void	(__stdcall *pThreadMemberFunc)(void* pObj, void *pParam);
typedef void	(__stdcall *pThreadFunc)(void *pParam);

class ThreadTask
{
public:
	template<typename FromType>
	static MethodPointer Bind(FromType method)//主要实现将成员函数指针转换成普通的指针，以便逃过编译器的类型匹配
	{
		union
		{
			FromType	_f;
			MethodPointer _t;
		}ut;
		ut._f = method;
		return ut._t;
	}
public:
	ThreadTask();
	virtual ~ThreadTask();
	bool	Accept(MethodPointer pfnMethod, void *pObj, void* pParam);
	bool	Accept(pThreadFunc	pfnFunc, void *pParam);
	bool	Run();
	void	Reset();
private:
	MethodPointer m_pfnMethodFunc;
	pThreadFunc	  m_pfnFunc;
	void*		  m_pParam;
	void*		  m_pObj;
};

class Thread
{
public:
	Thread();
	virtual ~Thread();
	bool	Run(MethodPointer pfnMethod, void *pObj, void* pParam);
	bool	Run(pThreadFunc	pfnFunc, void *pParam);
	bool	Run(ThreadTask task);
	void	WaitStop(unsigned long lMillSecond = INFINITE);
	DWORD*	GetThreadID();
	HANDLE	GetHanle();
	bool	IsRuning();
protected:
	bool Run();
	static DWORD __stdcall TMain(void *pParam);
	void	Main();
private:
	ThreadTask	m_tsTask;
	bool		m_bIsRun;
	HANDLE		m_hThread;
	DWORD		m_dwThreadID;
};

};//namespace XZBEN
#endif//__2013_07_28_THREAD_H__