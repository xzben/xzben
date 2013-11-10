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

#ifndef __2013_10_13_MUTEX_QUEUE_H__
#define __2013_10_13_MUTEX_QUEUE_H__

#include "Lock.h"
#include <queue>

namespace XZBEN
{
template<typename _ElementType>
class MutexQueue
{
public:
	MutexQueue()
	{

	}
	~MutexQueue()
	{

	}
	bool empty()
	{
		AutoLock lock(&m_mutex);
		return m_quData.empty();
	}
	int	size()
	{
		AutoLock lock(&m_mutex);
		return m_quData.size();
	}
	void push(const _ElementType& t)
	{
		AutoLock lock(&m_mutex);
		m_quData.push(t);
	}
	bool	front(_ElementType& value)
	{
		AutoLock lock(&m_mutex);
		if(m_quData.empty())  return false;
		value = m_quData.front();
		return true;
	}
	bool	front_pop(_ElementType& value)
	{
		AutoLock lock(&m_mutex);
		if( m_quData.empty() ) return false;
	
		value = m_quData.front();
		m_quData.pop();

		return true;
	}
	bool	back(_ElementType& value)
	{
		AutoLock lock(&m_mutex);
		if(m_quData.empty()) return false;
		value = m_quData.back();

		return true;
	}
	bool	back_pop(_ElementType& value)
	{
		AutoLock lock(&m_mutex);

		if( m_quData.empty() ) return false;
	
		value = m_quData.back();
		m_quData.pop();

		return true;
	}
	bool	top(_ElementType& value)
	{
		AutoLock lock(&m_mutex);

		if( m_quData.empty() ) return false;
		value = m_quData.top();

		return true;
	}
	bool	top_pop(_ElementType& value)
	{
		AutoLock lock(&m_mutex);

		if( m_quData.empty() ) return false;
		value = m_quData.top();
		m_quData.pop();

		return true;
	}
	void clear()
	{
		AutoLock lock(&m_mutex);

		if( !m_quData.empty() )
			m_quData.pop();
	}
private:
	std::queue<_ElementType>	m_quData;
	Mutex						m_mutex;
};

};//namespace XZBEN
#endif//__2013_10_13_MUTEX_QUEUE_H__