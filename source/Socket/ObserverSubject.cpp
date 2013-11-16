/************************************************************************************
 *	File   Name:																	*
 * 	Description:																	*	
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#include "ObserverSubject.h"
#include <algorithm>
#include "NetSocket.h"

namespace XZBEN
{
ObserverSubject::ObserverSubject()
{

}

ObserverSubject::~ObserverSubject()
{

}

void ObserverSubject::Notify(STATUS status, ShareNetSocketPtr pConnect)
{
	ObserverList::iterator it, itEnd;
	AutoLock lock(&m_mutex);
	for(it = m_observerList.begin(), itEnd = m_observerList.end(); it != itEnd; it++)
	{
		Observer *pObserver = *it;
		if(nullptr == pObserver) continue;
		pObserver->Update(status, pConnect);
	}
}

bool ObserverSubject::Attach(Observer *pObj)
{
	AutoLock lock(&m_mutex);
	ObserverList::iterator it = std::find(m_observerList.begin(), m_observerList.end(), pObj);
	if(it != m_observerList.end())
		return false;

	m_observerList.push_back(pObj);
	return true;
}

bool ObserverSubject::Dettach(Observer *pObj)
{
	AutoLock lock(&m_mutex);
	ObserverList::iterator it = std::find(m_observerList.begin(), m_observerList.end(), pObj);
	if(it != m_observerList.end())
	{
		m_observerList.erase(it);
		return true;
	}
	return false;
}

};//namespace XZBEN
