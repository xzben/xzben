/************************************************************************************
 *	File   Name: IOBuffer.h															*
 * 	Description: 用户存储IO数据的Buffer,其特性有:									*
 *				  1、可直接读取或写入任意合理大小的数据，但是预分配的空间不能超过	*
 *					 一个IOBlock的最大空间。										*			
 *				  2、GetPrepareWriteBuffer要与WriteBufferFinish配对使用				*
 *																					*
 *	Create Time: 08/07/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_07_IOBUFFER_H__
#define __2013_08_07_IOBUFFER_H__

#include "IOBlock.h"
#include "MemoryPool.h"
#include "Lock.h"
#include <vector>

namespace XZBEN
{
template<int MemoryPoolID>
class IOBuffer
{
public:
	typedef		IOBlock<MemoryPoolID>			IOBlockTemplate;
	typedef		std::vector<IOBlockTemplate*>	IOBufferQueue;

	IOBuffer()
	{
		m_pCurBlock = nullptr;
		m_nDataSize = 0;
	}

	virtual ~IOBuffer()
	{
		ClearIOBlocks();
	}
	int ReadData(void* pBuffer, int nLength, bool bDel)
	{
		if( nLength <= 0 ) return 0;

		void* pDataBuf = pBuffer;
		int   nReadLen = 0;

		AutoLock lock(&m_mutex);
		if(nLength > m_nDataSize)
			nLength = m_nDataSize;

		IOBufferQueue::iterator itBlock = m_IoBlocks.begin();
		while( itBlock != m_IoBlocks.end() )
		{
			IOBlockTemplate* pBlock = *itBlock;

			assert(nullptr != pBlock);

			if( (nullptr != pBlock) )
			{
				int nSize = pBlock->ReadData(pDataBuf, nLength - nReadLen, bDel);
				if(nullptr != pDataBuf)
					pDataBuf = (char*)pDataBuf + nSize;
				else
					nSize *= -1;

				nReadLen += nSize;
				if(bDel)
					m_nDataSize -= nSize;

				if(nReadLen >= nLength) break;

				if(bDel)
				{
					itBlock = m_IoBlocks.erase(itBlock);
					if(m_pCurBlock == pBlock)
						m_pCurBlock = nullptr;
					ReleaseBlock(pBlock);
				}
				else
					itBlock++;
			}
		}
		return nullptr == pBuffer ? -nReadLen : nReadLen;
	}
	bool GetDataBuffer(void *&pBuffer, int &nDataSize)
	{
		pBuffer = nullptr; nDataSize = 0;

		AutoLock lock(&m_mutex);

		IOBufferQueue::iterator itBlock = m_IoBlocks.begin();
		while( itBlock != m_IoBlocks.end() )
		{
			IOBlockTemplate *pBlock = *itBlock;
			assert(nullptr != pBlock);
			if( pBlock->GetDataBuffer(pBuffer, nDataSize) )
				return true;

			itBlock = m_IoBlocks.erase(itBlock);
			if(pBlock == m_pCurBlock)
				m_pCurBlock = nullptr;
			ReleaseBlock(pBlock);
		}
		return false;
	}

	bool GetEmptyBuffer(void *&pBuffer, int &nDataSize, bool bNewBlock = false)
	{
		pBuffer = nullptr; nDataSize = 0;
		
		AutoLock lock(&m_mutex);
		if(bNewBlock)
			m_pCurBlock = CreateBlock();
		else
		{
			if(nullptr == m_pCurBlock || m_pCurBlock->GetWriteAbleSize() <= 0)
				m_pCurBlock = CreateBlock();
		}
		
		return m_pCurBlock->GetEmptyBuffer(pBuffer, nDataSize);
	}
	int WriteData(void* pBuffer, int nLength)
	{
		if( nLength <= 0 ) return 0;

		void* pDataBuf = pBuffer;
		int   nWriteLen = 0;

		AutoLock lock(&m_mutex);

		if( nullptr == m_pCurBlock)
			m_pCurBlock = CreateBlock();

		while(1)
		{
			int nSize = m_pCurBlock->WriteData(pDataBuf, nLength - nWriteLen);
			if(nullptr != pDataBuf)
				pDataBuf  = (char*)pDataBuf + nSize;
			else
				nSize *= -1;

			nWriteLen += nSize;
			m_nDataSize += nSize;
			
			if(nWriteLen >= nLength) break;

			m_pCurBlock = CreateBlock();
		}

		return nullptr == pBuffer ? -nWriteLen : nWriteLen;
	}

	int	 GetDataSize()
	{
		AutoLock lock(&m_mutex);
		return m_nDataSize;
	}
private:
	void ClearIOBlocks()
	{
		AutoLock lock(&m_mutex);
		m_IoBlocks.clear();
	}
	void	ReleaseBlock(IOBlockTemplate *&pBlock)
	{
		SAFE_DELETE(pBlock);
	}
	IOBlockTemplate* CreateBlock()
	{
		IOBlockTemplate* pNewBlock = new IOBlockTemplate;
		m_IoBlocks.push_back(pNewBlock);
		return pNewBlock;
	}
private:
	IOBufferQueue		m_IoBlocks;
	int					m_nDataSize;
	IOBlockTemplate	   *m_pCurBlock;
	Mutex				m_mutex;
};
};//namespace XZBEN
#endif//__2013_08_07_IOBUFFER_H__