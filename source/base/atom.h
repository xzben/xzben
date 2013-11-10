/************************************************************************************
 *	File   Name: atom.h																*
 * 	Description: 此文件是从MDK框架中直接提取过来的，主要提供全局用于操作原子量的函数* 													
 *																					*
 *	Create Time: 08/04/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_04_ATOM_H__
#define __2013_08_04_ATOM_H__

#include "Base_system.h"

namespace XZBEN
{
class Atom
{
public:
	/*
	*	function : 原子量自加 1
	*	parameter: var 要被增加的原子量地址
	*	return:  
	*		WIN32: 返回自增后的值
	*/
	static inline uint32 AtomSelfAdd(void * var) 
	{
#ifdef WIN32
		return InterlockedIncrement((long *)(var)); // NOLINT
#else
		return __sync_add_and_fetch((uint32 *)(var), 1); // NOLINT
#endif
	}

	/*
	*	function : 原子量自减 1
	*	parameter: var 要被操作的原子量地址
	*	return:  
	*		WIN32: 返回自减后的值
	*/
	static inline uint32 AtomSelfDec(void * var) 
	{
#ifdef WIN32
		return InterlockedDecrement((long *)(var)); // NOLINT
#else
		return __sync_add_and_fetch((uint32 *)(var), -1); // NOLINT
#endif
	}

	/*
	*	function : 原子量增加指定值
	*	parameter: var 要被操作的原子量地址， value 要增加的值
	*	return:  
	*		WIN32: 返回增加前的值
	*/
	static inline uint32 AtomAdd(void * var, const uint32 value) 
	{
#ifdef WIN32
		return InterlockedExchangeAdd((long *)(var), value); // NOLINT
#else
		return __sync_fetch_and_add((uint32 *)(var), value);  // NOLINT
#endif
	}

	/*
	*	function : 原子量减小指定值
	*	parameter: var 要被操作的原子量地址， value 要减小的值
	*	return:  
	*		WIN32: 返回减小前的值
	*/
	static inline uint32 AtomDec(void * var, int32 value) 
	{
		value = value * -1;
#ifdef WIN32
		return InterlockedExchangeAdd((long *)(var), value); // NOLINT
#else
		return __sync_fetch_and_add((uint32 *)(var), value);  // NOLINT
#endif
	}

	/*
	*	function : 原子量设置成指定的值
	*	parameter: var 要被操作的原子量地址， value 要被设置的值
	*	return:  
	*		WIN32: 返回设置前的值
	*/
	static inline uint32 AtomSet(void * var, const uint32 value) 
	{
#ifdef WIN32
		::InterlockedExchange((long *)(var), (long)(value)); // NOLINT
#else
		__sync_lock_test_and_set((uint32 *)(var), value);  // NOLINT
#endif
		return value;
	}

	/*
	*	function : 获取原子量的值
	*	parameter: var 要被操作的原子量地址
	*	return:  
	*		WIN32: 原子量的当前值
	*/
	static inline uint32 AtomGet(void * var) 
	{
#ifdef WIN32
		return InterlockedExchangeAdd((long *)(var), 0); // NOLINT
#else
		return __sync_fetch_and_add((uint32 *)(var), 0);  // NOLINT
#endif
	}
private:
	Atom(){};
	~Atom(){}
};


};//namespace XZBEN
#endif//__2013_08_04_ATOM_H__