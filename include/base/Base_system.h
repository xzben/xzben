/************************************************************************************
 *	File   Name: defines.h															*
 * 	Description:																	*		
 *				定义socket底层系统相关的宏定义，使框架的时候不要考虑系统			*														
 *																					*
 *	Create Time: 07/26/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_07_26_XZBEN_SYSTEM_H__
#define __2013_07_26_XZBEN_SYSTEM_H__
#include<string>
#include<stdarg.h>
#include <cassert>
#include "Base_types.h"
#include "macros.h"

#ifdef WIN32
//添加 _WINSOCKAPI_ 以不避免包含 winsock.h文件的内容，因为 它会和 winsock2.h的内容有冲突
#define _WINSOCKAPI_
#include<Windows.h>
#endif

namespace XZBEN
{
class GlobalFunction
{
public:
	static inline std::string Format(char* format, ...)
	{
		char szFormatStr[8192];
		va_list arg;
		va_start(arg, format);
		vsprintf_s(szFormatStr, 8192, format, arg);
		va_end(arg);
		return szFormatStr;
	}
	static inline uint64	IpAddree2Int64(const std::string strIpAddress, uint16 nPort)
	{
		uint64 nIpAddress;
		uint8  buf[8];
		sscanf_s(strIpAddress.c_str(), "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]);
		memset(&nIpAddress, 0, sizeof(nIpAddress));

		buf[4] = nPort & 0x00ff;
		buf[5] = (nPort>>8);
		memcpy(&nIpAddress, buf, sizeof(buf));

		return nIpAddress;
	}
	static inline void		Int642IpAddress(uint64 nIpAddress, std::string& strIp, uint16& nPort) 
	{
		uint8  buf[8];
		memcpy(buf, &nIpAddress, sizeof(buf));
		char szIp[20];
		sprintf_s(szIp, 20,"%d.%d.%d.%d", buf[0], buf[1], buf[2], buf[3]);
		strIp = szIp;
		nPort = buf[5];
		nPort = (nPort<<8) + buf[4];
	}
private:
	GlobalFunction(){};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
};//namespace XZBEN
#endif//__2013_07_26_XZBEN_SYSTEM_H__