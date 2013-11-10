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

#ifndef __2013_10_27_OBSERVER_H__
#define __2013_10_27_OBSERVER_H__

#include "../base/Base_system.h"

namespace XZBEN
{
typedef uint16						STATUS;
enum OBServerStatus
{
	OBS_UNKNOW = -1,
};

class NetSocket;
class Observer
{
public:
	friend class ObserverSubject;
	Observer(){};
	virtual ~Observer(){};
protected:
	virtual bool Update(STATUS status, NetSocket *pNetSocket) = 0;
};

};//namespace XZBEN
#endif//__2013_10_27_OBSERVER_H__