/************************************************************************************
 *	File   Name: File.h														*
 * 	Description:  						* 													
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/
#ifndef __2013_10_22_FILE_H__
#define __2013_10_22_FILE_H__

#include <cstdio>
#include <string>

namespace XZBEN
{

class File	
{
public:
	enum{
		modeNull		= 0x0000,
		modeRead		= 0x0001,
		modeWrite		= 0x0002,
		modeAppend		= 0x0004,
		modeReadBinary  = 0x0008,
		modeWriteBinary = 0x0010,
	};
	enum{
		SeekStart = SEEK_SET,
		SeekEnd   = SEEK_END,
		SeekCur   = SEEK_CUR,
	};
	File();
	virtual ~File();
	bool Open(const char *strFilePath, int flag);
	bool Close();
	int	 Read(char* buffer, int len);
	int  Write(char* buffer, int len);
	bool ReadLine(char *buffer, int nMaxLen);
	bool WriteLine(char *buffer);
	bool Seek(int offset, int fromWhere);
	bool bIsEnd();
	long FileSize();
private:
	bool GetFileMode(int flag, char *strMode); 
private:
	FILE*			m_pFileHandle;
};

};//namespace XZBEN
#endif//__2013_10_22_FILE_H__