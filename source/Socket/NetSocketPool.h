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

#ifndef __2013_10_27_NECTCONNECTPOOL_H__
#define __2013_10_27_NECTCONNECTPOOL_H__

#include "../base/Lock.h"
#include "Socket.h"
#include <map>
#include <list>

namespace XZBEN
{
class NetSocket;
class NetTcp;
class NetUdp;
typedef std::map<SOCKET, NetTcp*>	NetTcpMap;
typedef std::map<SOCKET, NetUdp*>	NetUdpMap;
typedef std::list<NetUdp*>	WaitNetServerUdpList;
class NetSocketPool
{
public:
	NetSocketPool();
	virtual ~NetSocketPool();

	bool	AddNetTcp(NetTcp *pNetTcp);
	bool	DelNetTcp(NetTcp *pNetTcp);
	bool	DelNetTcp(SOCKET nID);
	NetTcp*	FindNetTcp(SOCKET nID);

	bool	AddDisNetTcp(NetTcp *pNetTcp);
	bool	DelDisNetTcp(NetTcp *pNetTcp);

	bool	AddNetUdp(NetUdp *pNetUdp);
	bool	SwapNetUdp(NetUdp *pNetUdp);
	bool	DelNetUdp(NetUdp *pNetUdp, bool bDel = true);
	bool	DelNetUdp(SOCKET nID, bool bDel = true);
	NetUdp*	FindNetUdp(SOCKET nID);

	bool	CheckTcpHeart(int16 nSecond);
	bool	CheckUdpHeart(int16 nSecond);
	bool	ReConnectTcpConnect();
	void	ClearPool();
private:
	Mutex			m_mutex;
	NetTcpMap		m_mpTcpConnect;
	NetTcpMap		m_mpTcpDisconnect;
	NetUdpMap		m_mpNetUdp;
	WaitNetServerUdpList		m_mpWaitNetServrUdp;
};


};//namespace XZBEN
#endif//__2013_10_27_NECTCONNECTPOOL_H__