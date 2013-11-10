/************************************************************************************
 *	File   Name: MemoryPoolManager.h												*
 * 	Description: 内存池管理类，系统内存池的分配由其管理								* 													
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_12_MEMORY_POOL_MANAGER_H__
#define __2013_08_12_MEMORY_POOL_MANAGER_H__

#include "MemoryPool.h"
#include "Lock.h"
#include <string>
#include <map>
namespace XZBEN
{
enum MemoryPoolIDS
{
	MPID_SOCK = 1,
	MPID_WIN,
};
class MemoryPoolManager
{
public:
	typedef std::map<int, MemoryPool*> MemoryQueue;
	static MemoryPoolManager* Single();
public:
	MemoryPool*	GetMemoryPool(int MemoryPoolID);
	~MemoryPoolManager();
protected:
	MemoryPoolManager();
	MemoryPool*	FindMemory(int MemoryPoolID);
	MemoryPool*	NewMemory(int MemoryPoolID);
	void DestoryMeoryPool();
private:
	MemoryQueue					m_MemoryQueue;
	Mutex						m_mutex;
};

};//namespace XZBEN
#endif//__2013_08_12_MEMORY_POOL_MANAGER_H__