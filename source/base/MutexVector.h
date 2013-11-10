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

#ifndef __2013_10_13_MUTEX_VECTOR_H__
#define __2013_10_13_MUTEX_VECTOR_H__

#include "Lock.h"
#include <vector>
namespace XZBEN
{
template<typename _ElementType>
class MutexVector
{
public:
	typedef	std::vector<_ElementType>::iterator			iterator;
	typedef	std::vector<_ElementType>::const_iterator	const_iterator;
	typedef	std::vector<_ElementType>::reference		reference;
	typedef std::vector<_ElementType>::reverse_iterator reverse_iterator;		
	MutexVector()
	{

	}
	MutexVector(int nSize)
		:m_vData(nSize)
	{

	}
	MutexVector(int nSize, const _ElementType& t)
		:m_vData(nSize, t)
	{

	}
	MutexVector(const MutexVector& mutexVector)
		m_vData(mutexVector.m_vData)
	{

	}
	~MutexVector()
	{

	}

	void push_back(const _ElementType& t)
	{
		AutoLock lock(&m_mutex);
		m_vData.push_back(t);
	}
	iterator	insert(iterator it, const _ElementType& t)
	{
		AutoLock lock(&m_mutex);
		return m_vData.insert(it, t);
	}
	void		insert(iterator it, int n, const _ElementType& t)
	{
		AutoLock lock(&m_mutex);
		m_vData.insert(it, n, t);
	}
	void	insert(iterator it, const_iterator first, const_iterator last)
	{
		AutoLock lock(&m_mutex);
		m_vData.insert(it, first, last);
	}
	iterator	erase(iterator it)
	{
		AutoLock lock(&m_mutex);
		return m_vData.erase(it);
	}
	iterator	erase(iterator first, iterator last)
	{
		AutoLock lock(&m_mutex);
		return m_vData.erase(first, last);
	}
	void pop_back()
	{
		AutoLock lock(&m_mutex);
		m_vData.pop_back();
	}
	void clear()
	{
		AutoLock lock(&m_mutex);
		m_vData.clear();
	}
	reference	at(int pos)
	{
		AutoLock lock(&m_mutex);
		return m_vData.at(pos);
	}
	reference front()
	{
		AutoLock lock(&m_mutex);
		return m_vData.front();
	}
	reference back()
	{
		AutoLock lock(&m_mutex);
		return m_vData.back();
	}
	iterator	begin()
	{
		AutoLock lock(&m_mutex);
		return m_vData.begin();
	}
	iterator    end()
	{
		AutoLock lock(&m_mutex);
		return m_vData.end();
	}
	reverse_iterator rbegin()
	{
		AutoLock lock(&m_mutex);
		return m_vData.rbegin();
	}
	reverse_iterator rend()
	{
		AutoLock lock(&m_mutex);
		return m_vData.rend();
	}
	bool empty()
	{
		AutoLock lock(&m_mutex);
		return m_vData.empty();
	}
	int	size()
	{
		AutoLock lock(&m_mutex);
		return m_vData.size();
	}
private:
	std::vector<_ElementType>	m_vData;
	Mutex						m_mutex;
};	

};//namespace XZBEN
#endif//__2013_10_13_MUTEX_VECTOR_H__