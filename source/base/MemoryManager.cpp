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

#include"MemoryPoolManager.h"

namespace XZBEN
{
MemoryPoolManager* MemoryPoolManager::Single()
{
	static MemoryPoolManager PoolManager;
	return &PoolManager;
}
////////////////////////////////////////////////////////////////
MemoryPoolManager::MemoryPoolManager()
{

}

MemoryPoolManager::~MemoryPoolManager()
{
	DestoryMeoryPool();
}
MemoryPool*	MemoryPoolManager::GetMemoryPool(int MemoryPoolID)
{
	MemoryPool *pMemoryPool = FindMemory(MemoryPoolID);
	if(NULL == pMemoryPool)
		pMemoryPool = NewMemory(MemoryPoolID);
	return pMemoryPool;
}

MemoryPool*	MemoryPoolManager::NewMemory(int MemoryPoolID)
{
	AutoLock lock(&m_mutex);
	MemoryPool *pMemoryPool =  new MemoryPool(SIZE1G);
	m_MemoryQueue.insert(MemoryQueue::value_type(MemoryPoolID, pMemoryPool));

	return pMemoryPool;
}
MemoryPool*	MemoryPoolManager::FindMemory(int MemoryPoolID)
{
	AutoLock lock(&m_mutex);
	MemoryQueue::iterator it = m_MemoryQueue.find(MemoryPoolID);
	if(it == m_MemoryQueue.end()) return NULL;

	return it->second;
}
void MemoryPoolManager::DestoryMeoryPool()
{
	AutoLock lock(&m_mutex);
	MemoryQueue::iterator it = m_MemoryQueue.begin();
	for(;it != m_MemoryQueue.end(); it++)
	{
		MemoryPool *pPool = it->second;
		SAFE_DELETE(pPool);
	}
	m_MemoryQueue.clear();
}
};//namespace XZBEN

