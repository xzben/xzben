/************************************************************************************
 *	File   Name: AlloFromMemoryPool.h												*
 * 	Description: 此类主要用于被继承,使子类 new 操作为从指定的内存池中分配空间       *
 *	Create Time: 08/07/2013															*
 *		 Author: xzben																*
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_07_ALLOCATE_MEMORY_FROM_MEMORY_POOL_H__
#define __2013_08_07_ALLOCATE_MEMORY_FROM_MEMORY_POOL_H__

#include "MemoryPool.h"
#include "MemoryPoolManager.h"

namespace XZBEN
{
/*
*	此类用来是被继承用，是子类是从 指定内存池中分配内存
*/
template<int MemoryPoolID>
class AllocFromMemoryPool
{
public:
	void*	operator new(size_t size)
	{
		return MemoryPoolManager::Single()->GetMemoryPool(MemoryPoolID)->Alloc(size);
	}
	void*	operator new[](size_t size)
	{
		return MemoryPoolManager::Single()->GetMemoryPool(MemoryPoolID)->Alloc(size);
	}
	void	operator delete(void *pobj)
	{
		MemoryPoolManager::Single()->GetMemoryPool(MemoryPoolID)->Free(pobj);
	}
	void	operator delete[](void *pobj)
	{
		MemoryPoolManager::Single()->GetMemoryPool(MemoryPoolID)->Free(pobj);
	}
	virtual ~AllocFromMemoryPool()
	{

	}
protected:  //讲构造函数定义为保护类型，使此类不能被直接声明对象，只能被继承使用
	AllocFromMemoryPool()
	{
		
	}
};
};//namespace XZBEN
#endif//__2013_08_07_ALLOCATE_MEMORY_FROM_MEMORY_POOL_H__