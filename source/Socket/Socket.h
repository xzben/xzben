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

/*
*	Socket 类封装了Socket常见的一些操作
*/
class Socket
{
public:
	/*
	*	加载socket动态库（windows only的操作）
	*	@para:	nHigh、nLow:	要加载的socket动态库的版本如默认为 1.1的库
	*/
	static bool			S_LoadSockLib(int nHigh = 1, int nLow = 1);
	/*
	*	卸载socket动态库（windows only的操作）
	*/
	static bool			S_DestroySockLib();
	/*
	*	从一个uint64型的地址结构获取对应的socketaddr_in 地址结构
	*	可以通过 GlobalFunction::IpAddree2Int64 将指定的ip地址，端口 获取uint64的地址
	*	可以通过 GlobalFunction::Int642IpAddress 将制定的uint64 地址转换成ip地址和端口	
	*/
	static sockaddr_in	S_GetSockAddrIpV4(uint64 nAddress);
	/*
	*	获取指定ip地址和端口号对应的sockaddr_in 地址
	*/
	static sockaddr_in	S_GetSockAddrIpV4(const char* szIpAddr, const u_short usPort);
	/*
	*	获取指定socketaddr_in 结构中的ip地址和端口
	*/
	static void			S_GetSockAddrIpV4(const sockaddr_in addr, std::string &szIpAddr,  u_short &usPort);
	/*
	*	获取一个随机的可用端口号
	*/
	static uint16		S_GetRandPort();
private:
	static	bool		s_bLoadedSockLib;
public:
	Socket();
	/*
	*	@para	epType:		socket的协议类型，定义在EProtocl中的TCP、UDP之类
	*	@para	nAf			socket采用的地址协议族 默认为 INET
	*	@para	nProtocol	socket采用协议
	*/
	Socket(const EProtocl epType, const int nAf = AF_INET, const int nProtocl = 0);
	virtual ~Socket();

	/*
	*	对TCP有效，获取socket的本地地址信息
	*/
	bool	GetLocalAddr(std::string& strIpAddr, u_short& usPort);
	/*
	*	对TCP有效，获取socket TCP连接对象地址信息
	*/
	bool	GetPeerAddr(std::string& strIpAddr, u_short& usPort);
	/*
	*	将socket关联到指定的socket句柄上
	*	@para	hSocket 要关联的socket
	*	@para	epType 要关联对象的类型 TCP、UDP
	*/
	bool	Attach(SOCKET hSocket, EProtocl epType);
	/*
	*	解除Socket对象和soket句柄的关联，这样在socket对象析构时不会去释放socket句柄对应的资源
	*/
	SOCKET	Dettach();
	/*
	*	将套接字绑定到指定地址上
	*/
	bool	Bind(const sockaddr_in& addrBind);
	/*
	*	对TCP有效，获取socket的本地地址信息
	*/
	bool	Listen(int nBacklog = 5);
	/*
	*	配置Socket开始TCP服务或UDP服务所需的操作
	*	TCP包含Bind和Listen
	*	UDP包含Bind
	*	@para	epType: TCP 或 UDP
	*	@para	nPort:	服务的端口号
	*	@return: 成功返回true，否则返回false
	*/
	bool	StartServer(EProtocl epType, int nPort);
	/*
	*	接收一个TCP客户连接请求，并返回建立连接的Socket类
	*/
	Socket	Accept();
	/*
	*	TCP连接到指定地址
	*/
	bool	Connect(const sockaddr_in& addrCon);
	//TCP 接收/发送数据
	int		Recv(char* pBuf, int nLen, int nFlag =0);
	int		Send(const char *pBuf,const int nLen, int nFlag = 0);
	//UDP 接收/发送数据
	int		RecvFrom(char* pBuf, int nLen, sockaddr_in addFrom, int nFlag =0);
	int		SendTo(const char *pBuf,const int nLen, sockaddr_in addrTo,int nFlag = 0);
	//关闭socket套接字
	bool	Close();
	SOCKET	GetSocket();
	//获取当前Socket类封装的是哪种类型的Socket：tcp/udp
	EProtocl GetType();
	//设置socket通信对方的地址
	bool	SetPeerAddr(const sockaddr_in& addrPeer);
	//TCP 连接建立时可以通过此函数获取连接对象的地址
	bool	InitPeerAddress();
	//用于IOCP accpet完成时更新socket的accept的Context, 只有这样才能通过InitPeerAddress正确获取连接对象的地址
	bool	UpdateAcceptContext();
	//用于IOCP connect完成时更新socket的connect context
	bool	UpdateConnectContext();
	//设置socket bind的端口是可被重复bind的
	bool	SetReuseAddr();
	//同socket 的shutdown
	bool	Shutdown(int how);
private:
	SOCKET			m_hSocket;	//套接字句柄
	sockaddr_in		m_addrPeer; //套接字连接后，与之连接的地址
	EProtocl		m_Protocl;  //套接字类型 upd， tcp
	bool			m_bHaveAddr;//是否包含连接对象的地址
};
};//namespace XZBEN

#endif//__2013_07_23_SOCKET_H__