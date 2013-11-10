/************************************************************************************
 *	File   Name: MemoryPool.h														*
 * 	Description: 提供一个内存池类，此类是根据appache内存池的原理编写的				*
 *				 主要原理为：														*
 *																					*
 *	Create Time: mm/dd/YYYY															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/
/*
*	本模块是参考 Apache 内存池而来 
*/
#ifndef __2013_08_03_MEMERY_POOL_H__
#define __2013_08_03_MEMERY_POOL_H__

#include"Lock.h"
#include "Base_types.h"
#include<ctime>
#include "Base_system.h"

namespace XZBEN
{
enum MemSize
{
	SIZE1K = 1024,
	SIZE1M = 1024 * SIZE1K,
	SIZE1G = 1024 * SIZE1M,
};
//basic memory node structure
struct apr_memnode_t{
	uint32			magic;			//用于标记这个内存块是内存池申请的			    
	apr_memnode_t	*next;			//指向下一个内存空间节点
	apr_memnode_t	**ref;			//指向当前内存空间节点
	uint32			index;			//当前内存空间节点的总共内存大小
	uint32			free_index;		//当前内存空间中的可用空间
	char*			first_avail;	//指向当前可用空间的起始地址　　	　　　
	char*			endp;			//指向当前可用地址的结束地址　
};
class Allocator
{
public:
	enum{
		APR_ALLOCATOR_MAX_FREE_UNLIMITED = 0,
		DEFAULT_ALIGN = 8,
		MAX_INDEX = 20,
		BOUNDARY_INDEX = 4, //决定分配器最小的分配单元大小使用时都是对 size >> BOUNDARY_INDEX 所以它代表4K的大小，其大小是分配器的大小基础单位
		BOUNDARY_SIZE =  (1 << BOUNDARY_INDEX), // 4K 分配的大小单位
		MIN_ALLOC = 2*BOUNDARY_SIZE,
	};
public:
	Allocator(size_t nMaxSize = APR_ALLOCATOR_MAX_FREE_UNLIMITED);
	virtual ~Allocator();
	inline const int GetMemNodeSize()
	{
		const int nMemNodeSize = Align(sizeof(apr_memnode_t), DEFAULT_ALIGN);
		return nMemNodeSize;
	}
	/*
	*	获得当前分配器分配的内存块的标记值
	*/
	inline uint32 GetMagic()
	{
		return m_uiMagic;
	}
	/*
	*	获得 nAllocSize 空间大小的节点
	*/
	apr_memnode_t*  Alloc(size_t nAllocSize);
	/*
	*	释放node节点的空间，注意这里的释放不一定会直接给系统回收
	*	可能是暂时留在分配器中，给下次要用的内存使用
	*/
	void Free(apr_memnode_t *node);
private:
	/*
	*	生成一个较大的随机数字
	*/
	static inline uint32	CreateMagic()
	{
		double start = 1, end = RAND_MAX;
		double uiMagic = (start + (end - start)*rand()/(RAND_MAX+1.0));
		uiMagic *= uiMagic;
		return (uint32)uiMagic;
	}
	/*
	*	function:	计算最接近nSize 的 nBoundary 的整数倍的整数，获得按指定字节对齐后的大小
	*	parameter:	nSize 为整数， nBoundary，必须为 2 的倍数
	*	example:	Align(7， 4) = 8，Align(21, 16) = 32
	*/
	static inline size_t Align(size_t nSize, size_t nBoundary)
	{
		return ((nSize +nBoundary-1) & ~(nBoundary - 1));
	}
	/*
	*	function:	设置分配子的最大内存分配空间限制，此设置关系到，
	*				当分配子中有多大内存时会将内存返回给系统回收
	*	paramter:	allocator : 要设置的分配子， nSize： 要设置的大小
	*	
	*/
	void inline SetMaxSize(size_t nSize)
	{
		//计算按 4K 对齐的空间大小后，获得其大小的4K倍数值
		uint32 uiMaxIndex = Align(nSize, BOUNDARY_SIZE) >> BOUNDARY_INDEX;
		
		//设置新的最大可存放空间大小，这操作要保证当前 m_uiCurAllocIndex(当前可存储在分配器中的内存大小)
		//做合理的调整，如果设置新最大值时，m_uiCurAllocIndex ==  m_uiMaxIndex 则要做相应的增加，
		//如果 m_uiCurAllocIndex < m_uiMaxIndex 那么加上这个差值也不会影响，因为 m_uiCurAllocIndex 会在后续的使用中
		//达到这个值。
		m_uiCurAllocIndex += uiMaxIndex - m_uiMaxIndex;
		m_uiMaxIndex = uiMaxIndex;

		if(m_uiCurAllocIndex > uiMaxIndex)
			m_uiCurAllocIndex = uiMaxIndex;
	}
	/*
	*	将分配器中挂载的空间全部给系统回收
	*/
	void Destroy();
private:
	uint32			m_uiMagic; //用于记录次分配器分配的内存块的标记值
	uint32			m_uiCurMaxBlockIndex; //分配器中当前可用的最大块的的大小index
	uint32			m_uiMaxIndex;//分配器可以存储的最大空间大小index
	uint32			m_uiCurAllocIndex;//当前已经分配的可留在分配器中的空间大小，其值总是在 m_uiMaxIndex范围内
	Mutex			*m_mutex;		 //多线程访问锁
	apr_memnode_t	*m_pfree[MAX_INDEX];//分配器当前挂载的可用内存块
};

class MemoryPool
{
public:
	MemoryPool(size_t nMaxSize = 0);
	virtual ~MemoryPool();
	virtual void* Alloc(size_t nAllocaSize);
	virtual bool Free(void* pMem);
private:
	Allocator	*m_pAllocator;
	Mutex		m_mutex;
};

};//namespace XZBEN
#endif//__2013_08_03_MEMERY_POOL_H__