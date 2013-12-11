/************************************************************************************
 *	File   Name: IOBlock.h															*
 * 	Description: IO通道中一个数据单元块												* 														
 *																					*
 *	Create Time: mm/dd/YYYY															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_07_IO_BLOCK_H__
#define __2013_08_07_IO_BLOCK_H__

#include "Lock.h"
#include "AllocFromMemoryPool.h"
namespace XZBEN
{
/*
*	IOBlock类为一个数据存储块，主要用来存储socket数据包
*	且此类的对象是从内存池分配的，所以在声明对象时要指定所用的内存池ID值，具体参照AllocFromMemoryPool的申明
*/
template<int MemoryPoolID>
class IOBlock : public AllocFromMemoryPool<MemoryPoolID>
{
public:
	enum	{MAX_BLOCK_SIZE = 8192 }; //block 块的大小值 byte为单位
	IOBlock()
	{
		m_nWriteIndex = 0;
		m_nReadIndex = 0;
	}
	~IOBlock()
	{

	}
	/*
	*	读取IOBlock中的内容
	*	@para	pBuffer: 存储读取的内容，可以为nullptr，也就是此次读取只是为了删除Block中的数据
	*	@para	nLenght: 要读取内容的长度
	*	@para	bDel   : 是否从Block中删除读取的内容
	*	@return:	成功读取的内容长度，如果pBuffer为nullptr则此长度为读取长度的负数
	*/
	int	ReadData(void * pBuffer, int nLength, bool bDel)
	{
		if(nLength <= 0)	return 0;

		AutoLock lock(&m_mutex);
		int nReadAbleSize = m_nWriteIndex - m_nReadIndex;

		if(nLength > nReadAbleSize )
			nLength = nReadAbleSize;

		if(nullptr != pBuffer)
			memcpy(pBuffer, &m_pBuffer[m_nReadIndex], nLength);
		if(bDel)
			m_nReadIndex += nLength;

		return (nullptr == pBuffer) ? - nLength : nLength;
	}
	/*
	*	向IOBlock中写入内容
	*	@para	pBuffer: 要写入的内容，可以为nullptr，也就是此次写入只是为了从block中预留一段空间
	*	@para	nLenght: 要写入内容的长度
	*	@return:	成功写入的内容长度，如果pBuffer为nullptr则此长度为写入长度的负数
	*/
	int	WriteData(void* pBuffer, int nLength)
	{
		if(nLength <= 0)  return 0;

		AutoLock lock(&m_mutex);

		int nWriteAbleSize = MAX_BLOCK_SIZE - m_nWriteIndex;
		if(nLength > nWriteAbleSize)
			nLength = nWriteAbleSize;

		if(nullptr != pBuffer)
			memcpy(&m_pBuffer[m_nWriteIndex], pBuffer, nLength);
		m_nWriteIndex += nLength;
		
		return (nullptr == pBuffer) ? -nLength : nLength;
	}
	/*
	*	获取当前Block中未被使用的buffer
	*	@para	pBuffer: 指向可用buffer的首地址
	*	@para	nLenght: 可用空间的size
	*	@return:	有可用空间返回true，否则返回false
	*/
	bool	GetEmptyBuffer(void *&pBuffer, int &nDataSize)
	{
		AutoLock lock(&m_mutex);

		nDataSize = MAX_BLOCK_SIZE - m_nWriteIndex;
		if(nDataSize <= 0) return false;
		pBuffer = (void*)&m_pBuffer[m_nWriteIndex];

		return true;
	}
	/*
	*	获取当前block还可以写入的buffer长度
	*	@return:	返回可写入的长度
	*/
	int		GetWriteAbleSize()
	{
		AutoLock lock(&m_mutex);
		return MAX_BLOCK_SIZE - m_nWriteIndex;
	}
	/*
	*	获取当前block中的已经写入数据的buffer
	*	@para	pBuffer: block中写入数据的buffer首地址
	*	@para	nDataSize: block中已经写入数据的buffer的size
	*	@return:	block中有写入数据返回true，否则返回false
	*/
	bool	GetDataBuffer(void *&pBuffer, int &nDataSize)
	{
		AutoLock lock(&m_mutex);
		nDataSize = m_nWriteIndex - m_nReadIndex;
		if(nDataSize <= 0) return false;
		pBuffer = (void*)&m_pBuffer[m_nReadIndex];
		return true;
	}
	/*
	*	获取当前block中的可以读取的数据size
	*	@return:	block中可读取数据的size
	*/
	int		GetReadAbleSize()
	{
		AutoLock lock(&m_mutex);
		return m_nWriteIndex - m_nReadIndex;
	}

private:
	Mutex			m_mutex;
	char			m_pBuffer[MAX_BLOCK_SIZE];
	int				m_nWriteIndex; //block 当前的可写入位置
	int				m_nReadIndex;  //block 当前的可读取位置
};

};//namespace XZBEN
#endif//__2013_08_07_IO_BLOCK_H__