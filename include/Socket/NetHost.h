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

#ifndef __2013_10_27_NETHOST_H__
#define __2013_10_27_NETHOST_H__

#include "Observer.h"
#include "Socket.h"

namespace XZBEN
{
class IODriver;
class NetProtocol;
class NetSocket;
class NetTcp;
class NetUdp;

enum
{
	DEFAULT_HEART_GAP = 10, //默认的心跳检查间隔
	DEFAULT_RECONNECT_GAP = 10,//默认重连间隔
};

class NetHost : public Observer
{
public:
	NetHost(IODriver *pDriver = nullptr, NetProtocol *pProtocl = nullptr);
	virtual ~NetHost();

	//给 Host 装载 驱动 和 协议
	bool	SetupDriver(IODriver *pDriver);
	bool	SetupProtocol(NetProtocol *pProtocol);
	//启动/停止 服务
	bool	StartServer(int nThreadNum = -1, bool bHeartCheck = false, int16 nHeartGap = DEFAULT_HEART_GAP);
	bool	StopServer();
	void	ThreadLoop();//服务IO线程循环

	//tcp 相关操作
	bool	ListernTcpPort(uint16 nPort);
	SOCKET	Connect(const char* szIp, uint16 nPort, bool bAutoConnect = false);
	bool	SendTcpMsg(SOCKET hSock, void* pBuffer, int nDataSize);
	//udp 相关操作
	bool	ListernUdpPort(uint16 nPort);
	bool	SendUdpMSG(const char* szIp, uint16 nPort, void* pBuffer, int len);
	
protected:
	virtual bool IsReady();
	//virtual 监视对象反馈接口
	bool	Update(STATUS status, NetSocket* pNetSocket) override;
private:
	IODriver		*m_pDriver;
	NetProtocol		*m_pProtocl;
	bool			m_bStart;
};

};//namespace XZBEN
#endif//__2013_10_27_NETHOST_H__