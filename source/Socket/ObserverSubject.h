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

#ifndef __2013_10_27_OBSERVERSUBJECT_H__
#define __2013_10_27_OBSERVERSUBJECT_H__

#include "../Base/Lock.h"
#include "Observer.h"
//#include "NetSocket.h"
#include <list>

namespace XZBEN
{
typedef	std::list<Observer*>		ObserverList;
/*
*	观察模式 被观察对象基类
*/
class ObserverSubject
{
public:
	ObserverSubject();
	virtual ~ObserverSubject();

	void Notify(STATUS	status, ShareNetSocketPtr pConnect);
	bool Attach(Observer *pObj);
	bool Dettach(Observer *pObj);
private:
	Mutex				m_mutex;
	ObserverList		m_observerList;
};

};//namespace XZBEN
#endif//__2013_10_27_OBSERVERSUBJECT_H__