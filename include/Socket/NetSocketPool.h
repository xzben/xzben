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
#include "../Base/SharePtr.h"
#include "Socket.h"
#include <map>
#include <list>

namespace XZBEN
{
class NetSocket;
class NetTcp;
class NetUdp;
typedef SharePtr<NetSocket>		ShareNetSocketPtr;
typedef SharePtr<NetTcp>		ShareNetTcpPtr;
typedef SharePtr<NetUdp>		ShareNetUdpPtr;

typedef std::map<SOCKET, ShareNetTcpPtr>	NetTcpMap;
typedef std::map<SOCKET, ShareNetUdpPtr>	NetUdpMap;
class NetSocketPool
{
public:
	NetSocketPool();
	virtual ~NetSocketPool();

	bool	AddNetTcp(ShareNetTcpPtr& pNetTcp);
	bool	DelNetTcp(ShareNetTcpPtr& pNetTcp);
	bool	DelNetTcp(SOCKET nID);
	ShareNetTcpPtr	FindNetTcp(SOCKET nID);

	bool	AddDisNetTcp(ShareNetTcpPtr& pNetTcp);
	bool	DelDisNetTcp(ShareNetTcpPtr& pNetTcp);

	bool	AddNetUdp(ShareNetUdpPtr& pNetUdp);
	bool	DelNetUdp(ShareNetUdpPtr& pNetUdp);
	bool	DelNetUdp(SOCKET nID);
	ShareNetUdpPtr	FindNetUdp(SOCKET nID);

	bool	CheckTcpHeart(int16 nSecond);
	bool	CheckUdpHeart(int16 nSecond);
	bool	ReConnectTcpConnect();
	void	ClearPool();
private:
	Mutex			m_mutex;
	NetTcpMap		m_mpTcpConnect;
	NetTcpMap		m_mpTcpDisconnect;
	NetUdpMap		m_mpNetUdp;
};


};//namespace XZBEN
#endif//__2013_10_27_NECTCONNECTPOOL_H__