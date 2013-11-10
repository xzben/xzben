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

#include "IODriver.h"
#include "NetSocket.h"
#include <algorithm>

namespace XZBEN
{
IODriver::IODriver()
{
	m_bIsRun = false;
	VERIFY( Socket::S_LoadSockLib() );
}

IODriver::~IODriver()
{
	VERIFY( Socket::S_DestroySockLib() );
}

bool IODriver::Start(int nThreadNum /*= -1*/, bool bHeartCheck /*= false*/, int16 nHeartGap /*= DEFAULT_HEART_GAP*/)
{
	nThreadNum = ( -1 ==nThreadNum ) ? GetBestThreadNumber() : nThreadNum;
	m_bIsRun = true;
	
	if( !bHeartCheck ) nHeartGap = 0;
	VERIFY( m_autoConnectThread.Run(ThreadTask::Bind(&IODriver::HeartThread), this, &nHeartGap) );
	VERIFY( m_heartThread.Run(ThreadTask::Bind(&IODriver::ReConncetThread), this, nullptr) );
	return m_bIsRun = m_IOEventThreadPool.Run(nThreadNum ,ThreadTask::Bind(&IODriver::IOEvent), this, nullptr);
}

void IODriver::Stop()
{
	m_bIsRun = false;
	m_autoConnectThread.WaitStop(0);
	m_heartThread.WaitStop(0);
	m_IOEventThreadPool.WaitStop(0);
}

void IODriver::ThreadLoop()
{
	m_autoConnectThread.WaitStop();
	m_heartThread.WaitStop();
	m_IOEventThreadPool.WaitStop();
}

bool IODriver::AddTcpMonitor(SOCKET sock)
{
	if( !AddMonitor(sock) || !AddAccept(sock) )
		return false;
	return true;
}

bool IODriver::AddUdpMonitor(SOCKET sock)
{
	NetUdp *pNetUdp = new NetServerUdp(this, sock);
	if( !AddMonitor(sock) || !AddRecvFrom(pNetUdp) || !m_NetSockPool.AddNetUdp(pNetUdp))
		return false;

	return true;
}

bool IODriver::ListernTcpPort(uint16 nPort)
{
	if( IsListernPort(nPort) ) 
		return false;

	Socket socket(TCP);
	if( !socket.StartServer(TCP, nPort) || !AddTcpMonitor(socket.Dettach()) )
		return false;

	return AddListenPort(nPort);
}

bool IODriver::ListernUdpPort(uint16 nPort)
{
	if( IsListernPort(nPort))
		return false;

	Socket socket(UDP);
	if( !socket.StartServer(UDP, nPort) || !AddUdpMonitor(socket.Dettach()) )
		return false;
	
	return AddListenPort(nPort);
}

bool IODriver::AddRecvFrom(SOCKET sock)
{
	NetUdp *pNetUdp = new NetUdp(this, sock);

	if( !AddRecvFrom(pNetUdp) || !m_NetSockPool.AddNetUdp(pNetUdp) )
		return false;

	pNetUdp->SetRecvStatus(true);
	return true;
}
//Port list
bool IODriver::IsListernPort(uint16 nPort)
{
	AutoLock lock(&m_mutex);
	PortList::iterator it = std::find(m_PortList.begin(), m_PortList.end(), nPort);

	return it != m_PortList.end();
}

bool IODriver::AddListenPort(uint16 nPort)
{
	AutoLock lock(&m_mutex);
	m_PortList.push_back(nPort);

	return true;
}

SOCKET	IODriver::Connect(const char* szIp, uint16 nPort, bool bAutoConnect)
{
	NetTcp* pNetTcp = new NetTcp(this, bAutoConnect);
	if( !pNetTcp->Connect(szIp, nPort) || !AddRecv(pNetTcp) || !m_NetSockPool.AddNetTcp(pNetTcp))
	{
		delete pNetTcp;
		return INVALID_SOCKET;
	}

	return pNetTcp->GetSocket();
}

bool IODriver::SendTcpMSG(SOCKET hSocket, void *pBuffer, int nDataSize)
{
	NetTcp* pNetTcp = m_NetSockPool.FindNetTcp(hSocket);
	if(nullptr == pNetTcp)
		return false;

	return pNetTcp->SendMessage(pBuffer, nDataSize);
}

bool IODriver::SendUdpMSG(const char* szIp, uint16 nPort, void *pBuffer, int nLen)
{
	Socket socket(UDP);
	uint16 nBindPort = Socket::S_GetRandPort();
	socket.StartServer(UDP, nBindPort);
	SOCKET hSock = socket.Dettach();
	
	NetUdp* pNetUdp = new NetUdp(this, hSock, true);
	if( !pNetUdp->SetPeer(szIp, nPort) 
	 || !AddMonitor(hSock) 
	 || !AddRecvFrom(pNetUdp)
	 ||	!m_NetSockPool.AddNetUdp(pNetUdp))
	{
		SAFE_DELETE(pNetUdp);
		return false;
	}
	
	return pNetUdp->SendMessage(pBuffer, nLen);
}
bool IODriver::OnError(IO_EVENT &event)
{
	return true;
}

bool IODriver::OnAccept(IO_EVENT &event)
{
	AddAccept(event._sock);

	NetTcp *pNetTcp = new NetTcp(this, false, true);
	if( !pNetTcp->CreateConnect(event._client) || !m_NetSockPool.AddNetTcp(pNetTcp) )
		return false;

	Notify(IOS_ACCEPT, pNetTcp);

	return true;
}

bool IODriver::OnDisConnect(IO_EVENT &event)
{
	NetTcp *pNetTcp = m_NetSockPool.FindNetTcp(event._client);
	if(nullptr == pNetTcp) 
		return false;
	
	Notify(IOS_DISCONNECT, pNetTcp);

	return m_NetSockPool.DelNetTcp(pNetTcp);
}
bool IODriver::OnRecv(IO_EVENT &event)
{
	NetTcp* pNetTcp = m_NetSockPool.FindNetTcp(event._client);
	if(nullptr == pNetTcp)
		return false;
	
	if( !pNetTcp->RecvDone(event._nDataSize) )
		return false;

	Notify(IOS_RECV, pNetTcp);
	
	return true;
}
bool IODriver::OnSendOut(IO_EVENT &event)
{
	NetTcp* pNetTcp = m_NetSockPool.FindNetTcp(event._client);
	if(nullptr == pNetTcp)
		return false;

	if( !pNetTcp->SendOut(event._nDataSize) )
		return false;

	return true;
}

bool IODriver::OnRecvFrom(IO_EVENT &event)
{
	NetUdp *pNetUdp = m_NetSockPool.FindNetUdp(event._client);
	if(nullptr == pNetUdp)
		return false;
	
	if( !pNetUdp->SetPeer(event._udpClientAddress) || !pNetUdp->RecvDone(event._nDataSize) )
		return false;

	Notify(IOS_RECVFROM, pNetUdp);

	return true;
}

bool IODriver::OnSendToOut(IO_EVENT &event)
{
	NetUdp *pNetUdp = m_NetSockPool.FindNetUdp(event._client);
	if(nullptr == pNetUdp)
		return false;
	
	if( !pNetUdp->SendOut(event._nDataSize) )
		return false;

	return true;
}
//Thread Process Function
void __stdcall IODriver::IOEvent(void *pParam)
{
	 while(m_bIsRun)
	 {
		 IO_EVENT Event;
		 if( !WaitEvent(Event) ) continue;
		 switch(Event._nIOType)
		 {
		 case IOS_ERROR:
			 VERIFY(OnError(Event));
			 break;
		 case IOS_CLOSE:
			 return;
		 case IOS_ACCEPT:
			 VERIFY(OnAccept(Event));
			 break;
		 case IOS_DISCONNECT:
			 VERIFY(OnDisConnect(Event));
			 break;
		 case IOS_SEND:
			 VERIFY(OnSendOut(Event));
			 break;
		 case IOS_SENDTO:
			 VERIFY(OnSendToOut(Event));
			 break;
		 case IOS_RECV:
			  VERIFY(OnRecv(Event));
			  break;
		 case IOS_RECVFROM:
			  VERIFY(OnRecvFrom(Event));
			  break;
		 }
	 }
}
void	__stdcall IODriver::HeartThread(void *pParam)
{
	int16 nHeartGap = *(int16*)pParam;

	bool bNeedTcpHeartCheck = nHeartGap > 0;
	int	nCheckGap = DEFAULT_HEART_GAP*1000/2;
	while(m_bIsRun)
	{
		if(bNeedTcpHeartCheck)
			m_NetSockPool.CheckTcpHeart(nHeartGap);
		m_NetSockPool.CheckUdpHeart(DEFAULT_HEART_GAP);
		Sleep(nCheckGap);
	}
}
void	__stdcall IODriver::ReConncetThread(void *pParam)
{
	int nReconnectGap = DEFAULT_RECONNECT_GAP*1000;
	while(m_bIsRun)
	{
		m_NetSockPool.ReConnectTcpConnect();
		Sleep(nReconnectGap);
	}
}
};//namespace XZBEN
