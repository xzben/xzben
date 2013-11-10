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

#include "NetSocket.h"
#include <cmath>

namespace XZBEN
{
NetSocket::NetSocket(IODriver *pDriver, bool bHeartCheck)
	:m_pDriver(pDriver), m_bHeartCheck(bHeartCheck)
{
	m_bIsSending = false;
	m_bIsRecving = false;
	RefreshHeart();
}
NetSocket::NetSocket(IODriver *pDriver, EProtocl epType, bool bHeartCheck)
		: m_socket(epType), m_pDriver(pDriver), m_bHeartCheck(bHeartCheck)
{
	m_bIsSending = false;
	m_bIsRecving = false;
	RefreshHeart();
}

NetSocket::~NetSocket()
{

}
SOCKET NetSocket::GetSocket()
{
	AutoLock lock(&m_mutex);
	return m_socket.GetSocket();
}

bool NetSocket::IsSending()
{
	AutoLock lock(&m_mutex);
	return m_bIsSending;
}

bool NetSocket::IsRecving()
{
	AutoLock lock(&m_mutex);
	return m_bIsRecving;
}

bool NetSocket::GetLocalAddr(std::string& strIp, uint16& nPort)
{
	AutoLock lock(&m_mutex);
	return m_socket.GetLocalAddr(strIp, nPort);
}

bool NetSocket::GetPeertAddr(std::string& strIp, uint16& nPort)
{
	AutoLock lock(&m_mutex);
	return m_socket.GetPeerAddr(strIp, nPort);
}

bool NetSocket::AttachSocket(SOCKET sock, EProtocl epType)
{
	AutoLock lock(&m_mutex);
	return m_socket.Attach(sock, epType);
}

time_t NetSocket::GetLastHeart()
{
	AutoLock lock(&m_mutex);
	return m_tmLastHeart;
}

void NetSocket::RefreshHeart()
{
	AutoLock lock(&m_mutex);
	m_tmLastHeart = time(NULL);
}
bool NetSocket::NeedHeartCheck()
{
	AutoLock lock(&m_mutex);
	return m_bHeartCheck;
}

bool NetSocket::GetSendBuffer(void *&pBuffer, int &nLen)
{
	return m_SendBuffer.GetDataBuffer(pBuffer, nLen);
}
int NetSocket::GetDataSize()
{
	return m_RecvBuffer.GetDataSize();
}
int NetSocket::ReadMsg(void *pBuffer, int nDataSize, bool bDel /* = true */)
{
	RefreshHeart();
	return m_RecvBuffer.ReadData(pBuffer, nDataSize, bDel);
}
bool NetSocket::SetRecvStatus(bool bRecvStatus)
{
	AutoLock lock(&m_mutex);
	m_bIsRecving = bRecvStatus;
	return bRecvStatus;
}
bool NetSocket::IsReady()
{
	AutoLock lock(&m_mutex);
	return m_bReady;
}

bool NetSocket::SetReady(bool bReady)
{
	AutoLock lock(&m_mutex);
	bool bOld = m_bReady;
	m_bReady = bReady;
	return bOld;
}
//////////////////////////////////////////////////////////////////////////
//NetTcp
NetTcp::NetTcp(IODriver *pDriver, bool bAutoConnect /* = false */, bool bHeartCheck /*= false*/)
	:NetSocket(pDriver, TCP, bHeartCheck)
{
	m_bAutoConnect = bAutoConnect;
}

NetTcp::~NetTcp()
{

}

bool NetTcp::Connect(const char* szIp, uint16 nPort)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	if( !m_socket.SetPeerAddr( Socket::S_GetSockAddrIpV4(szIp, nPort) )
		|| !m_pDriver->AddMonitor( m_socket.GetSocket() ) )
		return false;
	
	return m_socket.Connect(Socket::S_GetSockAddrIpV4(szIp, nPort));
}

bool NetTcp::SendMessage(void *pBuffer, int nDataSize)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	int nSize = m_SendBuffer.WriteData(pBuffer, nDataSize);
	if(nSize != nDataSize) //write failed
	{
		nSize = abs(nSize);
		m_SendBuffer.ReadData(nullptr, nSize, true);
		return false;
	}
	if( !m_pDriver->AddSend(this) )
		return false;

	m_bIsSending = true;
	return true;
}

bool NetTcp::Reconnect()
{
	std::string strIp; uint16 nPort;
	if( !m_socket.GetPeerAddr(strIp, nPort) )
		return false;

	return Connect(strIp.c_str(), nPort);
}

bool NetTcp::NeedAutoReConnect()
{
	return m_bAutoConnect;
}

bool NetTcp::SendOut(int nLen)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);

	bool bRet = (-nLen == m_SendBuffer.ReadData(nullptr, nLen, true));
	m_bIsSending = false;
	
	if( m_SendBuffer.GetDataSize() <= 0 )
		return true;

	if( !m_pDriver->AddSend(this) )
		return false;

	m_bIsSending = true;
	
	return true;
}

bool NetTcp::RecvDone(int nLen)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	
	bool bRet =  (-nLen == m_RecvBuffer.WriteData(nullptr, nLen));
	m_bIsRecving = false;

	if( m_pDriver->AddRecv(this) )
		m_bIsRecving = true;

	return bRet;
}

bool NetTcp::CreateConnect(SOCKET hSocket)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	if(!m_socket.Attach(hSocket, TCP) ||  !m_socket.UpdateAcceptContext() || !m_socket.InitPeerAddress() )
		return false;

	if( !m_pDriver->AddMonitor(hSocket) || !m_pDriver->AddRecv(this) )
		return false;

	m_bIsRecving = true;
	return true;
}
bool NetTcp::GetWriteAbleBuffers(void *&pBuffer, int &nBufferSize)
{
	return m_RecvBuffer.GetEmptyBuffer(pBuffer, nBufferSize);
}
//////////////////////////////////////////////////////////////////////////
//NetUdp
NetUdp::NetUdp(IODriver *pDriver, bool bHeartCheck /*= false*/)
	:NetSocket(pDriver, UDP, bHeartCheck)
{

}

NetUdp::NetUdp(IODriver *pDriver, SOCKET sock, bool bHeartCheck /*= false*/)
	:NetSocket(pDriver, bHeartCheck)
{
	AttachSocket(sock, UDP);
}

NetUdp::~NetUdp()
{

}

bool NetUdp::SendMessage(void *pBuffer, int nDataSize)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	int nSize = m_SendBuffer.WriteData(pBuffer, nDataSize);
	if(nSize != nDataSize)
	{
		nSize = abs(nSize);
		m_SendBuffer.ReadData(nullptr, nSize, true);
		return false;
	}
	if( !m_pDriver->AddSendTo(this) )
		return false;

	m_bIsSending = true;
	return true;
}

bool NetUdp::SetPeer(sockaddr_in Peeraddr)
{
	AutoLock lock(&m_mutex);
	return m_socket.SetPeerAddr(Peeraddr);
}

bool NetUdp::SetPeer(const char* szIp, uint16 nPort)
{
	AutoLock lock(&m_mutex);
	return m_socket.SetPeerAddr(Socket::S_GetSockAddrIpV4(szIp, nPort));
}

bool NetUdp::RecvDone(int nLen)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	bool bRet = (-nLen == m_RecvBuffer.WriteData(nullptr, nLen));
	m_bIsRecving = false;

	if( m_pDriver->AddRecvFrom(this) )
		m_bIsRecving = true;

	return bRet;
}

bool NetUdp::SendOut(int nLen)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	bool bRet = ( -nLen == m_SendBuffer.ReadData(nullptr, nLen, true) );
	m_bIsSending = false;

	return bRet;
}
bool NetUdp::GetWriteAbleBuffers(void *&pBuffer, int &nBufferSize)
{
	return m_RecvBuffer.GetEmptyBuffer(pBuffer, nBufferSize, true);
}
//////////////////////////////////////////////////////////////////////////
//NetServerUdp
NetServerUdp::NetServerUdp(IODriver *pDriver, SOCKET sock)
	:NetUdp(pDriver, sock, false)
{
	m_socket.SetCloseSocket(true);
}

NetServerUdp::~NetServerUdp()
{

}

bool NetServerUdp::RecvDone(int nLen)
{
	RefreshHeart();
	AutoLock lock(&m_mutex);
	m_bHeartCheck = true;
	bool bRet = (-nLen == m_RecvBuffer.WriteData(nullptr, nLen));
	m_bIsRecving = false;

	NetUdp *pNetUdp = new NetServerUdp(m_pDriver, m_socket.GetSocket());
	
	if(!m_pDriver->m_NetSockPool.SwapNetUdp(pNetUdp) 
	 ||!m_pDriver->AddRecvFrom(pNetUdp) )
		return false;

	m_bIsRecving = true;

	return bRet;
}

bool NetServerUdp::SendOut(int nLen)
{
	//NetServerUdp 不需要处理SendOut。
	//因为它在RecvFrom后转变成普通Udp只有一次，send的机会
	//至于它的删除工作会在心跳检查中执行
	return true;
}
};//namespace XZBEN