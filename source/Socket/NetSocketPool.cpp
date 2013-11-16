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

#include "NetSocketPool.h"
#include "NetSocket.h"

namespace XZBEN
{

NetSocketPool::NetSocketPool()
{
	
}

NetSocketPool::~NetSocketPool()
{
	ClearPool();
}

void NetSocketPool::ClearPool()
{
	AutoLock lock(&m_mutex);
	//TcpConnect
	m_mpTcpConnect.clear();
	m_mpTcpDisconnect.clear();
	m_mpNetUdp.clear();
}

ShareNetTcpPtr	NetSocketPool::FindNetTcp(SOCKET nID)
{
	AutoLock lock(&m_mutex);

	NetTcpMap::iterator it = m_mpTcpConnect.find(nID);
	if(it == m_mpTcpConnect.end()) 
		return nullptr;

	return it->second;
}

bool NetSocketPool::AddNetTcp(ShareNetTcpPtr& pNetTcp)
{
	if(nullptr == pNetTcp) 
		return false;

	int nID = pNetTcp->GetID();

	if(nullptr != FindNetTcp(nID)) 
		return false;

	AutoLock lock(&m_mutex);
	m_mpTcpConnect.insert(NetTcpMap::value_type(nID, pNetTcp));
	return true;
}

bool NetSocketPool::DelNetTcp(SOCKET nID)
{
	AutoLock lock(&m_mutex);

	NetTcpMap::iterator it = m_mpTcpConnect.find(nID);
	if(it == m_mpTcpConnect.end())
		return false;
	
	ShareNetTcpPtr& pNetTcp = it->second;

	assert(nullptr != pNetTcp);
	if(nullptr == pNetTcp)
		return false;
	
	if( pNetTcp->NeedAutoReConnect() )
		AddDisNetTcp(pNetTcp);
	else
		DELETE_SHARE_PTR(pNetTcp);

	m_mpTcpConnect.erase(it);
	
	return true;
}

bool NetSocketPool::DelNetTcp(ShareNetTcpPtr& pNetTcp)
{
	if(nullptr == pNetTcp) 
		return false;
	
	int nID = pNetTcp->GetID();

	return DelNetTcp(nID);
}

ShareNetUdpPtr	NetSocketPool::FindNetUdp(SOCKET nID)
{
	AutoLock lock(&m_mutex);

	NetUdpMap::iterator it = m_mpNetUdp.find(nID);
	if(it == m_mpNetUdp.end()) 
		return nullptr;

	return it->second;
}

bool NetSocketPool::AddNetUdp(ShareNetUdpPtr& pNetUdp)
{
	if(nullptr == pNetUdp) 
		return false;

	int nID = pNetUdp->GetID();

	if(nullptr != FindNetUdp(nID)) 
	{
		return false;
	}
	AutoLock lock(&m_mutex);
	m_mpNetUdp.insert(NetUdpMap::value_type(nID, pNetUdp));
	return true;
}

bool NetSocketPool::AddDisNetTcp(ShareNetTcpPtr& pNetTcp)
{
	if(nullptr == pNetTcp)
		return false;

	AutoLock lock(&m_mutex);
	if(m_mpTcpDisconnect.end() == m_mpTcpDisconnect.find(pNetTcp->GetID()))
		return false;

	m_mpTcpDisconnect.insert(NetTcpMap::value_type(pNetTcp->GetID(), pNetTcp));
	return true;
}

bool NetSocketPool::DelDisNetTcp(ShareNetTcpPtr& pNetTcp)
{
	if(nullptr == pNetTcp)
		return false;

	AutoLock lock(&m_mutex);
	NetTcpMap::iterator it = m_mpTcpDisconnect.find(pNetTcp->GetID());
	if(m_mpTcpDisconnect.end() == it)
		return false;

	m_mpTcpDisconnect.erase(it);
	return true;
}
bool NetSocketPool::DelNetUdp(SOCKET nID)
{
	AutoLock lock(&m_mutex);

	NetUdpMap::iterator it = m_mpNetUdp.find(nID);
	if(it != m_mpNetUdp.end())
	{
		ShareNetUdpPtr& pNetUdp = it->second;
		DELETE_SHARE_PTR(pNetUdp);
		m_mpNetUdp.erase(it);
		return true;
	}
	return false;
}
bool NetSocketPool::DelNetUdp(ShareNetUdpPtr& pNetUdp)
{
	if(nullptr == pNetUdp) 
		return false;

	int nID = pNetUdp->GetID();

	return DelNetUdp(nID);
}

bool NetSocketPool::CheckTcpHeart(int16 nSecond)
{
	if(nSecond <= 0) return false;

	AutoLock lock(&m_mutex);

	NetTcpMap::iterator it = m_mpTcpConnect.begin();
	time_t CurTime = time(NULL);
	while( it != m_mpTcpConnect.end() )
	{
		ShareNetTcpPtr& pNetTcp = it->second;
		time_t LastHeart = pNetTcp->GetLastHeart();

		if( pNetTcp->NeedHeartCheck() && CurTime - LastHeart > nSecond)
		{
			DELETE_SHARE_PTR(pNetTcp);
			it = m_mpTcpConnect.erase(it);
			continue;
		}
		it++;
	}
	return true;
}

bool NetSocketPool::CheckUdpHeart(int16 nSecond)
{
	if(nSecond <= 0) return false;

	AutoLock lock(&m_mutex);
	NetUdpMap::iterator it = m_mpNetUdp.begin();
	time_t CurTime = time(NULL);
	while( it != m_mpNetUdp.end() )
	{
		ShareNetUdpPtr& pNetUdp = it->second;
		if(nullptr == pNetUdp)
		{
			it++;
			continue;
		}
		time_t LastHeart = pNetUdp->GetLastHeart();

		if( pNetUdp->NeedHeartCheck() && CurTime - LastHeart > nSecond)
		{
			DELETE_SHARE_PTR(pNetUdp);
			it = m_mpNetUdp.erase(it);
			continue;
		}

		it++;
	}

	return true;
}
bool NetSocketPool::ReConnectTcpConnect()
{
	AutoLock lock(&m_mutex);

	NetTcpMap::iterator itTemp = m_mpTcpDisconnect.begin();

	while( itTemp != m_mpTcpDisconnect.end() )
	{
		ShareNetTcpPtr& pNetTcp = itTemp->second;
		if(nullptr == pNetTcp)
		{
			assert(false);
			itTemp++;
			continue;
		}
		if(pNetTcp->NeedAutoReConnect() && pNetTcp->Reconnect() )
		{
			itTemp = m_mpTcpDisconnect.erase(itTemp);
			AddNetTcp(pNetTcp);
		}
		else
			itTemp++;
	}
	return true;
}

};//namespace XZBEN
