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

	bool	GetEmptyBuffer(void *&pBuffer, int &nDataSize)
	{
		AutoLock lock(&m_mutex);

		nDataSize = MAX_BLOCK_SIZE - m_nWriteIndex;
		if(nDataSize <= 0) return false;
		pBuffer = (void*)&m_pBuffer[m_nWriteIndex];

		return true;
	}

	int		GetWriteAbleSize()
	{
		AutoLock lock(&m_mutex);
		return MAX_BLOCK_SIZE - m_nWriteIndex;
	}

	bool	GetDataBuffer(void *&pBuffer, int &nDataSize)
	{
		AutoLock lock(&m_mutex);
		nDataSize = m_nWriteIndex - m_nReadIndex;
		if(nDataSize <= 0) return false;
		pBuffer = (void*)&m_pBuffer[m_nReadIndex];
		return true;
	}
	
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