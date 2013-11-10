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
	NetTcpMap::iterator itTcpConnect = m_mpTcpConnect.begin();
	while(itTcpConnect != m_mpTcpConnect.end())
	{
		NetTcp *pNetTcp = itTcpConnect->second;
		SAFE_DELETE(pNetTcp);
		itTcpConnect++;
	}
	//TcpDisconnect
	NetTcpMap::iterator itTcpDisConnect = m_mpTcpDisconnect.begin();
	while(itTcpDisConnect != m_mpTcpDisconnect.end())
	{
		NetTcp *pNetTcp = itTcpDisConnect->second;
		SAFE_DELETE(pNetTcp);
		itTcpDisConnect++;
	}
	//Udp
	NetUdpMap::iterator itNetUdp = m_mpNetUdp.begin();
	while(itNetUdp != m_mpNetUdp.end())
	{
		NetUdp *pNetUdp = itNetUdp->second;
		SAFE_DELETE(pNetUdp);
		itNetUdp++;
	}
	//UdpWait
	WaitNetServerUdpList::iterator itWaitUdp = m_mpWaitNetServrUdp.begin();
	while(itWaitUdp != m_mpWaitNetServrUdp.end())
	{
		NetUdp *pNetUdp = *itWaitUdp;
		SAFE_DELETE(pNetUdp);
		itWaitUdp++;
	}
	m_mpTcpConnect.clear();
	m_mpTcpDisconnect.clear();
	m_mpNetUdp.clear();
	m_mpWaitNetServrUdp.clear();
}

NetTcp*	NetSocketPool::FindNetTcp(SOCKET nID)
{
	AutoLock lock(&m_mutex);

	NetTcpMap::iterator it = m_mpTcpConnect.find(nID);
	if(it == m_mpTcpConnect.end()) 
		return nullptr;

	return it->second;
}

bool NetSocketPool::AddNetTcp(NetTcp *pNetTcp)
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
	
	NetTcp* pNetTcp = it->second;

	assert(nullptr != pNetTcp);
	if(nullptr == pNetTcp)
		return false;
	
	if( pNetTcp->NeedAutoReConnect() )
		AddDisNetTcp(pNetTcp);
	else
		SAFE_DELETE(pNetTcp);

	m_mpTcpConnect.erase(it);
	
	return true;
}

bool NetSocketPool::DelNetTcp(NetTcp *pNetTcp)
{
	if(nullptr == pNetTcp) 
		return false;
	
	int nID = pNetTcp->GetID();

	return DelNetTcp(nID);
}

NetUdp*	NetSocketPool::FindNetUdp(SOCKET nID)
{
	AutoLock lock(&m_mutex);

	NetUdpMap::iterator it = m_mpNetUdp.find(nID);
	if(it == m_mpNetUdp.end()) 
		return nullptr;

	return it->second;
}

bool NetSocketPool::AddNetUdp(NetUdp *pNetUdp)
{
	if(nullptr == pNetUdp) 
		return false;

	int nID = pNetUdp->GetID();

	if(nullptr != FindNetUdp(nID)) 
		return false;

	AutoLock lock(&m_mutex);
	m_mpNetUdp.insert(NetUdpMap::value_type(nID, pNetUdp));
	return true;
}

bool NetSocketPool::AddDisNetTcp(NetTcp *pNetTcp)
{
	if(nullptr == pNetTcp)
		return false;

	AutoLock lock(&m_mutex);
	if(m_mpTcpDisconnect.end() == m_mpTcpDisconnect.find(pNetTcp->GetID()))
		return false;

	m_mpTcpDisconnect.insert(NetTcpMap::value_type(pNetTcp->GetID(), pNetTcp));
	return true;
}

bool NetSocketPool::DelDisNetTcp(NetTcp *pNetTcp)
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
bool NetSocketPool::DelNetUdp(SOCKET nID, bool bDel /*= true*/)
{
	AutoLock lock(&m_mutex);

	NetUdpMap::iterator it = m_mpNetUdp.find(nID);
	if(it != m_mpNetUdp.end())
	{
		NetUdp* pNetUdp = it->second;
		if(bDel)  SAFE_DELETE(pNetUdp);
		m_mpNetUdp.erase(it);
		return true;
	}
	return false;
}
bool NetSocketPool::DelNetUdp(NetUdp *pNetUdp, bool bDel /*= true*/)
{
	if(nullptr == pNetUdp) 
		return false;

	int nID = pNetUdp->GetID();

	return DelNetUdp(nID, bDel);
}
bool NetSocketPool::SwapNetUdp(NetUdp *pNetUdp)
{
	if(nullptr == pNetUdp)
		return false;

	SOCKET nId = pNetUdp->GetID();
	NetUdp *pOldNetUdp = FindNetUdp(nId);
	if(nullptr == pOldNetUdp)  return false;

	DelNetUdp(nId, false);
	AddNetUdp(pNetUdp);

	AutoLock lock(&m_mutex);
	m_mpWaitNetServrUdp.push_back(pOldNetUdp);

	return true;
}
bool NetSocketPool::CheckTcpHeart(int16 nSecond)
{
	if(nSecond <= 0) return false;

	AutoLock lock(&m_mutex);

	NetTcpMap::iterator it = m_mpTcpConnect.begin();
	time_t CurTime = time(NULL);
	while( it != m_mpTcpConnect.end() )
	{
		NetTcp *pNetTcp = it->second;
		time_t LastHeart = pNetTcp->GetLastHeart();

		if( pNetTcp->NeedHeartCheck() && CurTime - LastHeart > nSecond)
		{
			SAFE_DELETE(pNetTcp);
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
		NetUdp *pNetUdp = it->second;
		if(nullptr == pNetUdp)
		{
			it++;
			continue;
		}
		time_t LastHeart = pNetUdp->GetLastHeart();

		if( pNetUdp->NeedHeartCheck() && CurTime - LastHeart > nSecond)
		{
			SAFE_DELETE(pNetUdp);
			it = m_mpNetUdp.erase(it);
			continue;
		}

		it++;
	}

	WaitNetServerUdpList::iterator itServerUdp = m_mpWaitNetServrUdp.begin();
	while(itServerUdp != m_mpWaitNetServrUdp.end())
	{	
		NetUdp *pNetUdp = *itServerUdp;
		if(nullptr == pNetUdp)
		{
			itServerUdp++;
			continue;
		}
		time_t LastHeart = pNetUdp->GetLastHeart();
		if( pNetUdp->NeedHeartCheck() && CurTime - LastHeart > nSecond)
		{
			SAFE_DELETE(pNetUdp);
			m_mpWaitNetServrUdp.erase(itServerUdp);
			itServerUdp = m_mpWaitNetServrUdp.begin();
			continue;
		}

		itServerUdp++;
	}
	return true;
}
bool NetSocketPool::ReConnectTcpConnect()
{
	AutoLock lock(&m_mutex);

	NetTcpMap::iterator itTemp = m_mpTcpDisconnect.begin();

	while( itTemp != m_mpTcpDisconnect.end() )
	{
		NetTcp* pNetTcp = itTemp->second;
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
