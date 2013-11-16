/************************************************************************************
 *	File   Name: 																	*
 * 	Description:																	*
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_09_15_MACROS_H__
#define __2013_09_15_MACROS_H__

#include <assert.h>

namespace XZBEN
{
#ifndef XZBEN_BASE_MACROS
	#define XZBEN_BASE_MACROS
	#define SAFE_DELETE(_ptr)   if(nullptr != _ptr){ delete _ptr; _ptr = nullptr; }		
#ifdef _DEBUG
	#define VERIFY(_express)	assert(_express)
#else
	#define	VERIFY(_express)	_express
#endif
#endif
}//namespace XZBEN
#endif//__2013_09_15_MACROS_H__