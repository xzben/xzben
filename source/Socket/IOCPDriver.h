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

#ifndef __2013_10_27_IOCPDRIVER_H__
#define __2013_10_27_IOCPDRIVER_H__

#include "IODriver.h"

namespace XZBEN
{
typedef struct _IOCP_OVERLAPPED
{
	/**
		* OVERLAPPED类型指针
		* 指向完成操作参数
		* 传递给AcceptEx()的最后一个参数
		* 传递给WSARecv()的第6个参数
		* GetQueuedCompletionStatus()返回的第4个参数
		*/
	OVERLAPPED _overlapped;
	/**
		* 指向存有连接进来的客户端局域网和外网地址的内存
		* 必须使用动态分配的内存块
		* 传递给AcceptEx()的第3个参数
		* 
		*/
	char _outPutBuf[sizeof(SOCKADDR_IN)*2+32];
	/**
		* 客户端局域网IP信息长度
		* 传递给AcceptEx()的第5个参数
		*/
	unsigned long _dwLocalAddressLength;
	/**
		* 客户端外网IP信息长度
		* 传递给AcceptEx()的第6个参数
		*/
	unsigned long _dwRemoteAddressLength;
	WSABUF _wsaBuffer;	//WSARecv接收缓冲数据,传递给WSARecv()的第2个参数
	SOCKET _sock;
	IOStatus _completiontype;//完成类型1recv 2send
	sockaddr_in _clientAddr;//udp 连接的地址
}IOCP_OVERLAPPED;
class IOCPDriver : public IODriver
{
public:
	
	IOCPDriver();
	//virtual 
	~IOCPDriver();
///////////////////////////////////////////////
//virtual
protected:
	uint16	GetBestThreadNumber()override;
	bool	WaitEvent(IO_EVENT& event)override;
	bool	AddMonitor(SOCKET	sock)override;
	//tcp
	bool	AddAccept(SOCKET	listenSock)override;
	bool	AddSend(NetSocket *pNetTcp) override;
	bool	AddRecv(NetSocket *pNetTcp) override;
	//udp
	bool    AddSendTo(NetSocket *pNetUdp) override;
	bool    AddRecvFrom(NetSocket *pNetUdp) override;
private:
	IOCP_OVERLAPPED* CreateOverlapped();
	void DeleteOverlapped(IOCP_OVERLAPPED *&pDel);
	
	bool	AddSend(SOCKET sock, void* buffer, int nDataSize);
	bool	AddRecv(SOCKET sock, void* buffer, int nDataSize);
	bool	AddSendTo(SOCKET sock, void* buffer, int nDataSize, uint64 nAddress);
	bool	AddRecvFrom(SOCKET sock, void* buffer, int nDataSize);
private:
	HANDLE	m_hCompletionPort;
};

};//namespace XZBEN
#endif//__2013_10_27_IOCPDRIVER_H__