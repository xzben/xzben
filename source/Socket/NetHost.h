/************************************************************************************
 *	File   Name: NetHost.h															*
 * 	Description: 																	*
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

enum emHostSettings
{
	DEFAULT_HEART_GAP = 3,				//默认的心跳检查间隔
	DEFAULT_SERVER_UDP_HEART_GAP = 2,	//Server Udp 心跳间隔
	DEFAULT_RECONNECT_GAP = 5,			//默认重连间隔
};

/**
*	NetHost 一个网络主机（服务器/客户端），此主机封装了网络通信中通信层的核心逻辑
*	使用方法:
*		1、实例化一个NetHost对象（在构造时配置IO驱动、协议逻辑--可选）
*		2、如果实例化NetHost对象时没有配置IO驱动、协议逻辑对象，则要进行配置(SetupDriver、SetupProtocol)
*		3、启动服务(StartServer)
*		4、设置要监听的端口（TCP/UDP）
*		5、在合适的地方StopServer
**/
class NetHost : public Observer
{
public:
	/*
	*	@Para	pDriver:
	*			网络主机选用的IO驱动框架，目前支持有 windows下的IOCP（IOCPDriver）
	*			默认为nullptr，可以在后面用SetupDriver设置
	*	@para	pProtocol:
	*			主机通信采用的协议实现对象
	*			默认为nullptr，可以在后面用SetupProtocol
	*/
	NetHost(IODriver *pDriver = nullptr, NetProtocol *pProtocol = nullptr);
	virtual ~NetHost();

	//给 Host 装载 驱动 和 协议，可参照构造函数的的说明, 注意的是不能重复设置，重复设置会返回false
	bool	SetupDriver(IODriver *pDriver);
	bool	SetupProtocol(NetProtocol *pProtocol);

	/*
	*	@fuction: 
	*		网络主机开始网络通信服务
	*	@para nThreadNum:	网络主机IO处理线程数，这里线程数目按照需要合理配置，要注意的是不是线程越多效率越高，如果不知道如何配置可以选择 -1默认配置(2*cpu+2)
	*	@para bHeartCheck:	配置是否对连接到网络主机的连接做心跳检查，如果为true，主机将会断开心跳间隔内没有心跳产生的连接，所以这是对TCP有用的配置
	*	@para nHeartGap	:	当bHeartCheck为true时才有效，设置心跳间隔单位为秒
	*	@return:	成功返回true
	*/
	bool	StartServer(int nThreadNum = -1, bool bHeartCheck = false, int16 nHeartGap = DEFAULT_HEART_GAP);
	/*
	*	强制停止网络主机的网络服务
	*/
	bool	StopServer();
	/*
	*	网络主机的IO处理线程循环。当网络服务停止后会跳出循环。
	*/
	void	ThreadLoop();//服务IO线程循环

	//tcp 相关操作
	/*
	*	监听指定的Tcp端口，在端口上等待连接
	*	@para	nPort: 要监听的Tcp端口号
	*/
	bool	ListenTcpPort(uint16 nPort);
	/*
	*	把网络主机连接到另外指定ip和端口的主机上
	*	@para	szIp: 要连接主机的IP地址
	*	@para	nPort: 要连接的主机端口号
	*	@para	bAutoConnect:	当与连接的主机断开后是否自动再次重新连接
	*	@return	:	连接成功返回连接成功的socket，否则返回 INVALID_SOCKET
	*/
	SOCKET	Connect(const char* szIp, uint16 nPort, bool bAutoConnect = false);
	/*
	*	向连接好的指定socket发送数据
	*	@para	hSocket: 连接成功后返回的socket标识
	*	@para	pBuffer: 要发送的数据buffer
	*	@para	nDataSize: 要发送的数据size
	*	@return : 发送成功返回true
	*/
	bool	SendTcpMsg(SOCKET hSock, void* pBuffer, int nDataSize);
	//udp 相关操作
	/*
	*	监听指定的udp端口，在端口上等待udp消息数据
	*	@para nPort:	要监听的端口号
	*	@return :成功返回true
	*/
	bool	ListenUdpPort(uint16 nPort);
	/*
	*	想指定ip地址、端口的主机发送udp数据
	*	@para	szIp:	udp数据目的ip地址
	*	@para	nPort	udp数据目的端口
	*	@para	pBuffer:udp数据buffer
	*	@para	len:	udp数据buffer的size
	*	@return	: 成功返回true
	*/
	bool	SendUdpMSG(const char* szIp, uint16 nPort, void* pBuffer, int len);
	
protected:
	virtual bool IsReady();
	//virtual 监视对象反馈接口
	bool	Update(STATUS status, ShareNetSocketPtr& pNetSocket) override;
private:
	IODriver		*m_pDriver;	
	NetProtocol		*m_pProtocl;
	bool			m_bStart;
};

}//namespace XZBEN
#endif//__2013_10_27_NETHOST_H__