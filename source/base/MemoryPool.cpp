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

#include "MemoryPool.h"

namespace XZBEN
{
/////////////////////////////////////////////////////////////////////////////////////////
//class Allocator public
Allocator::Allocator(size_t nMaxSize /*= APR_ALLOCATOR_MAX_FREE_UNLIMITED*/)
{
	m_uiMagic = CreateMagic();
	m_uiCurMaxBlockIndex = 0; //初始状态下，m_pfree[] 为空，所以没有最大可用块 
	m_uiMaxIndex = APR_ALLOCATOR_MAX_FREE_UNLIMITED;//初始状态为可存储空间无限
	m_uiCurAllocIndex = 0;//当前已经分配的可留在分配器中的空间大小，其值总是在 m_uiMaxIndex范围内
	m_mutex = new Mutex;
	memset(m_pfree, 0, sizeof(m_pfree));

	if(nMaxSize != APR_ALLOCATOR_MAX_FREE_UNLIMITED)
		SetMaxSize(nMaxSize);
}
Allocator::~Allocator()
{
	if(m_mutex)
			delete m_mutex;
		Destroy();
}
apr_memnode_t* Allocator::Alloc(size_t nAllocSize)
{
	apr_memnode_t *node, **ref;
	uint32 uiCurMaxBlockIndex;
	size_t nSize, i, index;

	const int nMemNodeSize = GetMemNodeSize();

	nSize = Align(nAllocSize + nMemNodeSize, BOUNDARY_SIZE);
	if(nSize < nAllocSize) //可能由于nAllocSize过大导致计算的nSize比nAllocSize小
	{
		return NULL;
	}
	if(nSize < MIN_ALLOC)
		nSize = MIN_ALLOC;

	//转换大小值为 以4K为单位的值，即其在free中所对应的index
	//由于nSize 最小为8K，所以这里计算的index 最小 = 1
	index = (nSize >> BOUNDARY_INDEX) - 1;
	if(index > UINT32_MAX) //申请的空间过大则不分配
	{
		return NULL;
	}
	if(index <= m_uiCurMaxBlockIndex)//当前存在可用的内存块够index
	{
		m_mutex->Lock();

		uiCurMaxBlockIndex = m_uiCurMaxBlockIndex;
		ref = &m_pfree[index];
		i = index;
		while(NULL == *ref && i < uiCurMaxBlockIndex)
			ref++, i++;

		if(NULL != (node = *ref))
		{
			if(NULL == (*ref = node->next) && i >= uiCurMaxBlockIndex)
			//如果找到的可用内存块是当前分配器中最大的块，且是最后一块最大块
			//则更新分配器中当前的可用最大块
			{
				do{
					ref--;
					uiCurMaxBlockIndex--;
				}while(NULL == *ref && uiCurMaxBlockIndex > 0);
				m_uiCurMaxBlockIndex = uiCurMaxBlockIndex;
			}

			m_uiCurAllocIndex += node->index + 1;
			if(m_uiCurAllocIndex > m_uiMaxIndex)
				m_uiCurAllocIndex = m_uiMaxIndex;
				
			m_mutex->Unlock();
			node->next = NULL;
			node->first_avail= (char*)node + nMemNodeSize;
			return node;
		}
		m_mutex->Unlock();
	}
	else if(m_pfree[0])
	{
		m_mutex->Lock();
		ref = &m_pfree[0];
		while(NULL != (node = *ref) && index > node->index)
			ref = &node->next;

		if(node)
		{
			*ref = node->next;
			m_uiCurAllocIndex += node->index + 1;
			if(m_uiCurAllocIndex > m_uiMaxIndex)
				m_uiCurAllocIndex = m_uiMaxIndex;
				
			m_mutex->Unlock();
			node->next = NULL;
			node->first_avail = (char*)node + nMemNodeSize;
			return node;
		}
		m_mutex->Unlock();
	}
	if(NULL == (node = (apr_memnode_t*)malloc(nSize)))
	{
		return NULL;
	}
	node->magic = m_uiMagic;
	node->next = NULL;
	node->index = index;
	node->first_avail = (char*)node + nMemNodeSize;
	node->endp = (char*)node + nSize;

	return node;
}
void Allocator::Free(apr_memnode_t *node)
{
	apr_memnode_t *next, *freelist = NULL;
	uint32 index, uiCurMaxBlockIndex;
	uint32 uiMaxIndex, uiCurAllocIndex;

	m_mutex->Lock();
	uiCurMaxBlockIndex = m_uiCurMaxBlockIndex;
	uiMaxIndex = m_uiMaxIndex;
	uiCurAllocIndex = m_uiCurAllocIndex;

	do{
		next = node->next;
		index = node->index;
			
		if(APR_ALLOCATOR_MAX_FREE_UNLIMITED != uiMaxIndex
			&& index + 1 > uiCurAllocIndex) //如果当前index + 1 空间是超出限定maxindex 的空间则将其删除
		{
			node->next = freelist;
			freelist = node;
		}
		else if(index < MAX_INDEX)
		{
			if(NULL == (node->next = m_pfree[index])
				&& index > uiCurMaxBlockIndex)
			{
				uiCurMaxBlockIndex = index;
			}
			m_pfree[index] = node;
			if(uiCurAllocIndex >= index + 1)
				uiCurAllocIndex -= index + 1;
			else
				uiCurAllocIndex = 0;
		}
		else
		{
			node->next = m_pfree[0];
			m_pfree[0] = node;
			if(uiCurAllocIndex >= index + 1)
				uiCurAllocIndex -= index + 1;
			else
				uiCurAllocIndex = 0;
					
		}
	}while(NULL != (node = next));
	m_uiCurMaxBlockIndex = uiCurMaxBlockIndex;
	m_uiCurAllocIndex = uiCurAllocIndex;
	
	m_mutex->Unlock();
	while(NULL != freelist)
	{
		node = freelist;
		freelist = node->next;
		free(node);
	}
}
void Allocator::Destroy()
{
	uint32 index;
	apr_memnode_t *node, **ref;

	for(index = 0; index < MAX_INDEX; index++)
	{
		ref = &m_pfree[index];
		while((node = *ref) != NULL){
			*ref = node->next;
			free(node);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
//class MemoryPool public
MemoryPool::MemoryPool(size_t nMaxSize /*= Allocator::APR_ALLOCATOR_MAX_FREE_UNLIMITED*/)
{
	m_pAllocator = new Allocator(nMaxSize);
}
void* MemoryPool::Alloc(size_t nAllocaSize)
{
	apr_memnode_t* node = m_pAllocator->Alloc(nAllocaSize);
	if(node == NULL)
	{
		return NULL;
	}
	return node->first_avail;
}
bool MemoryPool::Free(void* pMem)
{
	if(NULL == pMem)
	{
		return false;
	}
	apr_memnode_t* node = (apr_memnode_t*)((char*)pMem - m_pAllocator->GetMemNodeSize());
	if(node->magic != m_pAllocator->GetMagic()) //如果此段内存不是此内存池的分配器分配的
	{
		return false;
	}
	m_pAllocator->Free(node);
	return true;
}
MemoryPool::~MemoryPool()
{
	if(m_pAllocator)
		delete m_pAllocator;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
};//namespace XZBEN

