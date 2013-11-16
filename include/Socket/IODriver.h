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

#ifndef __2013_10_27_IODRIVER_H__
#define __2013_10_27_IODRIVER_H__

#include "../base/Lock.h"
#include "../base/ThreadPool.h"
#include "NetHost.h"
#include "NetSocketPool.h"
#include "ObserverSubject.h"
#include <list>

namespace XZBEN
{

enum IOStatus
{
	IOS_ERROR = OBS_UNKNOW,
	IOS_CLOSE,
	IOS_CONNECT,
	IOS_DISCONNECT,
	IOS_ACCEPT,
	IOS_SEND,
	IOS_SENDTO,
	IOS_SENDOUT,
	IOS_RECV,
	IOS_RECVFROM,
};

//IO 事件涉及资源
struct IO_EVENT
{
	IOStatus	_nIOType;
	SOCKET		_sock;
	SOCKET		_client;
	void*		_pBuffer;
	int			_nDataSize;
	sockaddr_in	_udpClientAddress; //udp 通信时，获取客户端的地址
};

class IODriver : public ObserverSubject
{
public:
	friend class NetTcp;
	friend class NetUdp;
	friend class NetServerUdp;
	typedef std::list<uint16>	PortList;
	IODriver();
	virtual ~IODriver();
	
	//启动/停止 服务
	bool	Start(int nThreadNum = -1, bool bHeartCheck = false, int16 nHeartGap = DEFAULT_HEART_GAP);
	void	Stop();
	void	ThreadLoop(); //IO 线程循环

	//tcp
	bool	ListenTcpPort(uint16 nPort);
	SOCKET	Connect(const char* szIp, uint16 nPort, bool bAutoConnect);
	bool	SendTcpMSG(SOCKET hSocket, void *pBuffer, int nDataSize);
	
	//udp
	bool	ListenUdpPort(uint16 nPort);
	bool	SendUdpMSG(const char* szIp, uint16 nPort, void *pBuffer, int nLen);
	
private:
	//端口nPort是否已经被监听
	bool	IsListenPort(uint16 nPort);
	//将端口加入监听列表
	bool	AddListenPort(uint16 nPort);
	//IO操作线程过程
	void	__stdcall IOEvent(void *pParam);
	//心跳处理线程过程
	void	__stdcall HeartThread(void *pParam);
	//重连处理线程过程
	void	__stdcall ReConncetThread(void *pParam);

protected:
//virtual 根据具体Driver实现的
	//获取Driver对应的最合适线程数
	virtual uint16	GetBestThreadNumber() = 0;
	//获取一个IO事件
	virtual	bool	WaitEvent(IO_EVENT& event) = 0;
	virtual bool	AddMonitor(SOCKET sock) = 0;
	//tcp
	virtual bool	AddAccept(SOCKET listenSock) = 0;
	bool			AddSend(ShareNetSocketPtr pNetTcp);
	virtual bool	AddSend(NetSocket *pNetTcp) = 0;
	bool			AddRecv(ShareNetSocketPtr pNetTcp);
	virtual bool	AddRecv(NetSocket *pNetTcp) = 0;
	//udp
	bool			AddSendTo(ShareNetSocketPtr pNetUdp);
	virtual bool    AddSendTo(NetSocket *pNetUdp) = 0;
	bool			AddRecvFrom(ShareNetSocketPtr pNetUdp);
	virtual bool    AddRecvFrom(NetSocket *pNetUdp) = 0;
private:
	//各种IO事件的对应处理借口
	bool	OnError(IO_EVENT &event);
	bool	OnAccept(IO_EVENT &event);
	bool	OnDisConnect(IO_EVENT &event);
	bool	OnRecv(IO_EVENT &event);
	bool	OnSendOut(IO_EVENT &event);
	bool	OnRecvFrom(IO_EVENT &event);
	bool	OnSendToOut(IO_EVENT &event);
private:
	Mutex			m_mutex;			//Driver 资源互斥锁
	ThreadPool		m_IOEventThreadPool;//io线程池
	Thread			m_heartThread;		//心跳线程
	Thread			m_autoConnectThread;//重连线程
	NetSocketPool	m_NetSockPool;		//socket 管理池
	PortList		m_PortList;			//监听列表
	bool			m_bIsRun;			//Driver是否运行
};

}//namespace XZBEN
#endif//__2013_10_27_IODRIVER_H__