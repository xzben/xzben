/************************************************************************************
 * 	Description:																	*
 *			the socket abstract base class 											* 														
 *																					*
 *	Create Time: 07/23/2013															*
 *		 Author: xzben																*	
 *																					*	
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#include"Socket.h"

namespace XZBEN
{
bool Socket::s_bLoadedSockLib = false;
bool Socket::S_LoadSockLib(int nHigh /* = 1 */, int nLow /* = 1 */)
{
#ifdef WIN32  // window 下才需要加载 winsock lib
	if( s_bLoadedSockLib ) //已经加载过，直接返回
		return true;

	s_bLoadedSockLib = true;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( nHigh, nLow );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
		return false;

	if ( LOBYTE( wsaData.wVersion ) != nHigh 
		|| HIBYTE( wsaData.wVersion ) != nLow )
	{
			WSACleanup( );
			return false;
	}
#endif
	return true;
}
bool Socket::S_DestroySockLib()
{
#ifdef WIN32
	if( !s_bLoadedSockLib ) //还未加载过，或已经卸载了
		return true; 

	s_bLoadedSockLib = false;
	if(0 != WSACleanup())
		return false;
#endif
	return true;
}
sockaddr_in	Socket::S_GetSockAddrIpV4(uint64 nAddress)
{
	std::string strIp; uint16 nPort;
	GlobalFunction::Int642IpAddress(nAddress, strIp, nPort);
	return S_GetSockAddrIpV4(strIp.c_str(), nPort);
}
sockaddr_in Socket::S_GetSockAddrIpV4(const char* szIpAddr, const u_short usPort)
{
	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(usPort);
	if(nullptr == szIpAddr)
		addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	else
		addr.sin_addr.S_un.S_addr = inet_addr(szIpAddr);

	return addr;
}
void	Socket::S_GetSockAddrIpV4(const sockaddr_in addr, std::string& strIpAddr, u_short &usPort)
{
	usPort = ntohs(addr.sin_port);
	strIpAddr = inet_ntoa(addr.sin_addr);
}
uint16	Socket::S_GetRandPort()
{
	sockaddr_in addr = S_GetSockAddrIpV4(NULL, 0);
	Socket sock(TCP);
	sock.Bind(addr);
	std::string strIp; uint16 nPort;
	sock.GetLocalAddr(strIp, nPort);
	sock.Close();

	return nPort;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
Socket::Socket()
{
	m_hSocket = INVALID_SOCKET;
	m_Protocl = EPUNKNOW;
	m_bHaveAddr = false;
	m_bNoCloseSocket = false;
	memset(&m_addrPeer, 0, sizeof(m_addrPeer));
}
Socket::Socket(const EProtocl epType, const int nAf /*= AF_INET*/, const int nProtocl /*= 0*/)
{
	m_hSocket = ::socket(nAf, epType, nProtocl);
	m_Protocl = epType;
	m_bHaveAddr = false;
	m_bNoCloseSocket = false;
	memset(&m_addrPeer, 0, sizeof(m_addrPeer));
}

Socket::~Socket()
{
	if(!m_bNoCloseSocket) Close();
}

bool Socket::Attach(SOCKET hSocket, EProtocl epType)
{
	if(INVALID_SOCKET == hSocket)
		return false;

	m_hSocket = hSocket;
	m_Protocl = epType;
	memset(&m_addrPeer, 0, sizeof(m_addrPeer));
	m_bHaveAddr = false;
	return true;
}

SOCKET Socket::Dettach()
{
	SOCKET skRet = m_hSocket;
	m_hSocket = INVALID_SOCKET;
	memset(&m_addrPeer, 0, sizeof(m_addrPeer));
	m_bHaveAddr = false;
	m_Protocl = EPUNKNOW;
	return skRet;
}

bool Socket::SetPeerAddr(const sockaddr_in& addrPeer)
{
	m_addrPeer = addrPeer;
	m_bHaveAddr = true;
	return true;
}

bool Socket::SetReuseAddr()
{
	int opt=1;
	return ( SOCKET_ERROR != setsockopt(m_hSocket,
		SOL_SOCKET,SO_REUSEADDR,
		(char*)&opt,
		sizeof(opt)) );
}

bool Socket::Bind(const sockaddr_in& addrBind)
{
	if( SOCKET_ERROR == ::bind(m_hSocket, (sockaddr*)&addrBind, sizeof(sockaddr)))
	{
		int nErr = ::GetLastError();
		//常见的err 为 WSAEADDRINUSE  10048
		//SetValid(false, GlobalFunction::Format("Bind address faild error id: %d", nErr));
		return false;
	}
	return true;
}

bool Socket::Listen(int nBacklog /*= 5*/)
{
	if(SOCKET_ERROR == ::listen(m_hSocket, nBacklog))
	{
		int nErr = ::GetLastError();
		//常见的err 为 WSAEADDRINUSE  10048
		//SetValid(false, GlobalFunction::Format("listen faild error id: %d", nErr));
		return false;
	}
	return true;
}

bool Socket::StartServer(EProtocl epType, int nPort)
{
	sockaddr_in addr = S_GetSockAddrIpV4(nullptr, nPort);
	if(!Bind(addr))  return false;

 	return (UDP == epType) ? true : Listen();
}

Socket	Socket::Accept()
{
	Socket connSocket;
	sockaddr_in	conAddr;
	memset(&conAddr, 0, sizeof(conAddr));
	int nAddrLen = sizeof(conAddr);

	SOCKET hConnSocket = ::accept(m_hSocket, (sockaddr*)&conAddr, &nAddrLen);
	if(INVALID_SOCKET != hConnSocket)
	{
		connSocket.Attach(hConnSocket, TCP);
		connSocket.SetPeerAddr(conAddr);
	}
	
	return connSocket;
}
bool	Socket::Connect(const sockaddr_in& addrCon)
{
	if( SOCKET_ERROR == ::connect(m_hSocket, (sockaddr*)&addrCon, sizeof(addrCon)) )
	{
		int nErr = ::GetLastError();
		//SetLastError(GlobalFunction::Format("Connect failed error id: %d", nErr));
		return false;
	}
	SetPeerAddr(addrCon);
	return true;
}

int	Socket::Recv(char* pBuf, int nLen, int nFlag /* =0*/)
{
	int nSize = ::recv(m_hSocket, pBuf, nLen, nFlag);
	if(nSize == 0)
	{
		//SetLastError("recv from a closed connect.");
		return SE_SOCKET_CLOSED;
	}
	else if(nSize == SOCKET_ERROR)
	{
		//SetLastError( GlobalFunction::Format("recv data failed error id:%d", ::GetLastError() ) );
		return SE_ERROR;
	}
	return nSize;
}
int	Socket::Send(const char *pBuf,const int nLen, int nFlag/* =0*/)
{
	int nSize;
	if( SOCKET_ERROR == (nSize = ::send(m_hSocket, pBuf, nLen, nFlag)) )
	{
		int nErr = ::GetLastError();
		//SetLastError(GlobalFunction::Format("Send data failed error Id: %d", nErr));
		return SE_ERROR;
	}
	return nSize;
}
int Socket::RecvFrom(char* pBuf, int nLen, sockaddr_in addFrom, int nFlag /*=0*/)
{
	int nFrom = sizeof(addFrom);
	int nSize = ::recvfrom(m_hSocket, pBuf, nLen, nFlag, (sockaddr*)&addFrom, &nFrom);
	if(nSize == 0)
	{
		//SetLastError("recvfrom from a closed connect.");
		return SE_SOCKET_CLOSED;
	}
	else if(nSize == SOCKET_ERROR)
	{
		//SetLastError( GlobalFunction::Format("recvfrom data failed error id:%d", ::GetLastError() ) );
		return SE_ERROR;
	}
	SetPeerAddr(addFrom);
	return nSize;
}
int Socket::SendTo(const char *pBuf,const int nLen, sockaddr_in addrTo,int nFlag /*= 0*/)
{
	int nSize = ::sendto(m_hSocket, pBuf, nLen, nFlag, (sockaddr*)&addrTo, sizeof(addrTo));
	if(SOCKET_ERROR == nSize)
	{
		int nErr = ::GetLastError();
		//SetLastError(GlobalFunction::Format("Send data failed error Id: %d", nErr));
		return SE_ERROR;
	}
	SetPeerAddr(addrTo);
	return nSize;
}

bool Socket::Close()
{
	if(INVALID_SOCKET == m_hSocket)
		return false;

	closesocket(m_hSocket);
	return true;
}

bool Socket::GetLocalAddr(std::string& strIpAddr, u_short& usPort)
{
	sockaddr_in addr;
	int nLen = sizeof(addr);
	if(0 == getsockname(m_hSocket, (sockaddr*)&addr, &nLen))
	{
		S_GetSockAddrIpV4(addr, strIpAddr, usPort);
		return true;
	}
	return false;
}

bool Socket::GetPeerAddr(std::string& strIpAddr, u_short& usPort)
{
	if(!m_bHaveAddr) return false;
	S_GetSockAddrIpV4(m_addrPeer, strIpAddr, usPort);
	return true;
}

SOCKET	Socket::GetSocket()
{
	return m_hSocket;
}

EProtocl Socket::GetType()
{
	return m_Protocl;
}

bool Socket::UpdateAcceptContext()
{
#ifdef WIN32
	return 0 == setsockopt( m_hSocket,
		SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char *)&(m_hSocket), sizeof(m_hSocket) );
#else
	return true;
#endif
}
bool	Socket::UpdateConnectContext()
{
#ifdef WIN32
	return 0 == setsockopt( m_hSocket,
		SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT,
		NULL, 0 );
#else
	return true;
#endif
}
bool Socket::InitPeerAddress()
{
	assert( INVALID_SOCKET != m_hSocket );
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);

	if ( SOCKET_ERROR == getpeername( m_hSocket,
		(sockaddr*)&sockAddr, &nSockAddrLen ) ) return false;

	m_addrPeer = sockAddr;
	m_bHaveAddr = true;
	return true;
}
bool Socket::Shutdown(int how)
{
	return 0 == shutdown(m_hSocket, how);
}

bool Socket::SetCloseSocket(bool bNoCloseSocket)
{
	bool bOldValue = m_bNoCloseSocket;
	m_bNoCloseSocket = bNoCloseSocket;

	return bOldValue;
}
};//namespace XZBEN

