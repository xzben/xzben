/************************************************************************************
 *	File   Name: Socket.h															*
 * 	Description: 提供一个网络通信套接字的封装类，将套接字设计的主要操作封装起来		*
 *	Create Time: 08/06/2013															*
 *		 Author: xzben																*	
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_08_06_SOCKET_H__
#define __2013_08_06_SOCKET_H__

#include <string>

#ifdef WIN32
	#include<Winsock2.h>
	#include <MSWSock.h>
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment ( lib, "mswsock.lib")
#endif //WIN32

#include "../Base/Lock.h"

namespace XZBEN
{
enum EProtocl
{
	EPUNKNOW = -1,
	TCP = SOCK_STREAM,
	UDP = SOCK_DGRAM,
};

enum ErrorID{
	SE_ERROR = -3,
	SE_SOCKET_CLOSED,
};
class Socket
{
public:
	static bool			S_LoadSockLib(int nHigh = 1, int nLow = 1);
	static bool			S_DestroySockLib();
	static sockaddr_in	S_GetSockAddrIpV4(uint64 nAddress);
	static sockaddr_in	S_GetSockAddrIpV4(const char* szIpAddr, const u_short usPort);
	static void			S_GetSockAddrIpV4(const sockaddr_in addr, std::string &szIpAddr,  u_short &usPort);
	static uint16		S_GetRandPort();
private:
	static	bool		s_bLoadedSockLib;
public:
	Socket();
	Socket(const EProtocl epType, const int nAf = AF_INET, const int nProtocl = 0);
	virtual ~Socket();
	bool	GetLocalAddr(std::string& strIpAddr, u_short& usPort);
	bool	GetPeerAddr(std::string& strIpAddr, u_short& usPort);
	bool	Attach(SOCKET hSocket, EProtocl epType);
	SOCKET	Dettach();
	bool	Bind(const sockaddr_in& addrBind);
	bool	Listen(int nBacklog = 5);
	bool	StartServer(EProtocl epType, int nPort);
	Socket	Accept();
	bool	Connect(const sockaddr_in& addrCon);
	int		Recv(char* pBuf, int nLen, int nFlag =0);
	int		Send(const char *pBuf,const int nLen, int nFlag = 0);
	int		RecvFrom(char* pBuf, int nLen, sockaddr_in addFrom, int nFlag =0);
	int		SendTo(const char *pBuf,const int nLen, sockaddr_in addrTo,int nFlag = 0);
	bool	Close();
	SOCKET	GetSocket();
	EProtocl GetType();
	bool	SetPeerAddr(const sockaddr_in& addrPeer);
	bool	InitPeerAddress();
	bool	UpdateAcceptContext();
	bool	UpdateConnectContext();
	bool	SetReuseAddr();
	bool	Shutdown(int how);
	bool	SetCloseSocket(bool bNoCloseSocket);
private:
	SOCKET			m_hSocket;	//套接字句柄
	sockaddr_in		m_addrPeer; //套接字连接后，与之连接的地址
	EProtocl		m_Protocl;  //套接字类型 upd， tcp
	bool			m_bHaveAddr;//是否包含连接对象的地址
	bool			m_bNoCloseSocket;
};
};//namespace XZBEN

#endif//__2013_07_23_SOCKET_H__