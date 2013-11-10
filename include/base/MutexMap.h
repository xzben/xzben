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

#ifndef __2013_10_13_MUTEX_MAP_H__
#define __2013_10_13_MUTEX_MAP_H__

#include "Lock.h"
#include <map>
namespace XZBEN
{
template<typename key_tyep,typename value_type>
class MutexMap
{
public:
	typedef std::map<key_tyep, value_type>::iterator				iterator;
	typedef std::map<key_tyep, value_type>::const_iterator			const_iterator;
	typedef std::map<key_tyep, value_type>::reverse_iterator		reverse_iterator;
	typedef std::map<key_tyep, value_type>::reference				reference;
	typedef std::map<key_tyep, value_type>::size_type				size_type;
	typedef std::map<key_tyep, value_type>::value_type
	MutexMap()
	{

	}
	MutexMap(const MutexMap& mutexMap)
		:m_mpData(mutexMap.m_mpData)
	{

	}
	~MutexMap()
	{

	}
	iterator	insert(const value_type& x)
	{
		AutoLock lock(&m_mutex);
		return m_mpData.insert(x);
	}
	iterator	insert(iterator it, const value_type& x)
	{
		AutoLock lock(&m_mutex);
		return m_mpData.insert(it, x);
	}
	void insert(const value_type *first, const value_type *last)
	{
		AutoLock lock(&m_mutex);
		m_mpData.insert(first, last);
	}
	iterator erase(iterator it)
	{
		AutoLock lock(&m_mutex);
		return m_mpData.erase(it);
	}
	iterator erase(iterator first, iterator last)
	{
		AutoLock lock(&m_mutex);
		return m_mpData.erase(first, last);
	}
	size_type erase(const key_type& key)
	{
		AutoLock lock(&m_mutex);
		return m_mpData.erase(key);
	}
	iterator begin()
	{
		AutoLock lock(&m_mutex);
		return m_mpData.begin();
	}
	iterator end()
	{
		AutoLock lock(&m_mutex);
		return m_mpData.end();
	}
	reverse_iterator rbegin()
	{
		AutoLock lock(&m_mutex);
		return m_mpData.rbegin();
	}
	reverse_iterator rend()
	{
		AutoLock lock(&m_mutex);
		return m_mpData.rend();
	}
	int count()
	{
		AutoLock lock(&m_mutex);
		return m_mpData.count();
	}
	const_iterator find(const key_type& key)
	{
		AutoLock lock(&m_mutex);
		return m_mpData.find(key);
	}
	
	reference  operator[](const key_type& key)
	{
		AutoLock lock(&m_mutex);
		return m_mpData[key];
	}
	int size()
	{
		AutoLock lock(&m_mutex);
		return m_mpData.size();
	}
	bool empty()
	{
		AutoLock lock(&m_mutex);
		return m_mpData.empty();
	}
private:
	std::map<key_tyep, value_type>	m_mpData;
	Mutex							m_mutex;
};

};//namespace XZBEN
#endif//__2013_10_13_MUTEX_MAP_H__