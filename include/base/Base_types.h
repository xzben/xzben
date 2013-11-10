/************************************************************************************
 *	File   Name: FixLengthInt.h														*
 * 	Description: 																	* 														
 *																					*
 *	Create Time: 07/26/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_07_26_XZBEN_TYPES_H__
#define __2013_07_26_XZBEN_TYPES_H__

namespace XZBEN
{
typedef int						BOOL;
#ifndef	TRUE
	#define TRUE				1
#endif//TRUE
#ifndef FALSE
	#define FALSE				0
#endif//FALSE
#ifndef UINT32_MAX
	#define	UINT32_MAX				0xffffffffU
#endif//UINT32_MAX
#ifdef WIN32
	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned __int64	uint64;
	typedef char				int8;
	typedef short				int16;
	typedef int					int32;
	typedef __int64				int64;
#endif
}//namespace XZBEN
#endif//__2013_07_26_XZBEN_TYPES_H__