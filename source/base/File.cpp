#include "File.h"
#include <assert.h>

namespace XZBEN
{

File::File()
{
	m_pFileHandle = nullptr;
}

File::~File()
{
	Close();
}

bool File::Open(const char *strFilePath, int flag)
{
	char strMode[10];
	
	if(nullptr == strFilePath || !GetFileMode(flag, strMode) ) return false;

	m_pFileHandle = fopen(strFilePath, strMode);
	if(nullptr == m_pFileHandle)
		return false;
	return true;
}

bool File::Close()
{
	if(m_pFileHandle != nullptr)
	{
		if(EOF == fclose(m_pFileHandle)) 
			return false;
		m_pFileHandle = nullptr;
	}
	return false;
}

int File::Read(char* buffer, int len)
{
	assert(nullptr != m_pFileHandle);
	if(m_pFileHandle == nullptr) return 0;

	return  fread(buffer, 1, len, m_pFileHandle);
}
int File::Write(char* buffer, int len)
{
	assert(nullptr != m_pFileHandle);
	if(m_pFileHandle == nullptr) return 0;

	int nSize = fwrite(buffer, 1, len, m_pFileHandle);
	fflush(m_pFileHandle);
	return nSize;
}
bool File::ReadLine(char *buffer, int nMaxLen)
{
	assert(nullptr != m_pFileHandle);
	if(m_pFileHandle == nullptr) return false;

	return NULL != fgets(buffer, nMaxLen, m_pFileHandle);
}
bool File::Seek(int offset, int fromWhere)
{
	assert(nullptr != m_pFileHandle);
	if(m_pFileHandle == nullptr) return false;
	return 0 == fseek(m_pFileHandle, offset, fromWhere);
}
bool File::WriteLine(char *buffer)
{
	assert(nullptr != m_pFileHandle);
	if(m_pFileHandle == nullptr) return false;

	if( EOF == fputs(buffer, m_pFileHandle) || EOF == fputs("\n", m_pFileHandle))
		return false;

	return true;
}
bool File::bIsEnd()
{
	assert(nullptr != m_pFileHandle);
	if(m_pFileHandle == nullptr) return true;
	return 0 != feof(m_pFileHandle);
}
long  File::FileSize()
{
	assert(nullptr != m_pFileHandle);
	if(m_pFileHandle == nullptr) return 0;


	long nCurPos = ftell(m_pFileHandle);

	fseek(m_pFileHandle, 0, SeekEnd); 
	long nSize = ftell(m_pFileHandle);

	fseek(m_pFileHandle, nCurPos, SeekStart);
	return nSize;
}
/*
r 以只读方式打开文件，该文件必须存在。
r+ 以可读写方式打开文件，该文件必须存在。
rb+ 读写打开一个二进制文件，允许读写数据。
rw+ 读写打开一个文本文件，允许读和写。
w 打开只写文件，若文件存在则文件长度清为0，即该文件内容会消失。若文件不存在则建立该文件。
w+ 打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
a 以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）
a+ 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。 （原来的EOF符不保留）
wb 只写打开或新建一个二进制文件；只允许写数据。
wb+ 读写打开或建立一个二进制文件，允许读和写。
ab+ 读写打开一个二进制文件，允许读或在文件末追加数据。
at+ 打开一个叫string的文件，a表示append,就是说写入处理的时候是接着原来文件已有内容写入，不是从头写入覆盖掉，t表示打开文件的类型是文本文件，+号表示对文件既可以读也可以写。
*/
bool File::GetFileMode(int flag, char *strMode)
{
	 assert(nullptr != strMode);
	switch(flag)
	{
	case modeRead:  //文本只读
		sprintf(strMode, "r");
		return true;
	case modeWrite: //文本只写
		sprintf(strMode, "w");
		return true;
	case modeRead | modeWrite: //文本读写
		sprintf(strMode, "rw+");
		return true;
	case modeWrite | modeAppend:
		sprintf(strMode, "a");
		return true;
	case modeRead | modeWrite | modeAppend:
		sprintf(strMode, "a+");
		return true;
	case modeReadBinary:
		sprintf(strMode, "rb");
		return true;
	case modeWriteBinary:
		sprintf(strMode, "wb");
		return true;
	case modeWriteBinary | modeAppend:
		sprintf(strMode, "ab+");
		return true;
	case modeReadBinary | modeWriteBinary | modeAppend:
		sprintf(strMode, "ab+");
		return true;
	case modeReadBinary | modeWriteBinary:
		sprintf(strMode, "wb+");
		return true;
	}
	return false;
}
};//namespace XZBEN