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

#include "IOCPDriver.h"
#include "NetSocket.h"
#include "../Base/AllocFromMemoryPool.h"

namespace XZBEN
{
IOCPDriver::IOCPDriver()
{
	//创建完成端口，并制定最大并行线程数为cpu内核数
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	assert(NULL != m_hCompletionPort);
}

IOCPDriver::~IOCPDriver()
{

}

bool IOCPDriver::AddSend(NetSocket *pNetTcp)
{
	if( pNetTcp->IsSending() )
		return true;
	
	void *pBuffer; int nBufferSize;
	if( !pNetTcp->GetSendBuffer(pBuffer, nBufferSize) )
		return false;
	
	return AddSend(pNetTcp->GetSocket(), pBuffer, nBufferSize);
}

bool IOCPDriver::AddSendTo(NetSocket *pNetUdp)
{
	void *pBuffer; int nBufferSize;

	if( !pNetUdp->GetSendBuffer(pBuffer, nBufferSize) )
		return false;
	std::string strIp; uint16 nPort;
	pNetUdp->GetPeertAddr(strIp, nPort);
	uint64 nAddress = GlobalFunction::IpAddree2Int64(strIp, nPort);
	return AddSendTo(pNetUdp->GetSocket(), pBuffer, nBufferSize,  nAddress);
}

bool IOCPDriver::AddRecv(NetSocket *pNetTcp)
{
	if(pNetTcp->IsRecving())
		return true;

	SOCKET sock = pNetTcp->GetSocket();
	void *pBuffer; int nBufferSize;
	if( !pNetTcp->GetWriteAbleBuffers(pBuffer, nBufferSize) )
		return false;

	if( !AddRecv(sock, pBuffer, nBufferSize) )
		return false;
	return true;
}

bool IOCPDriver::AddRecvFrom(NetSocket *pNetUdp)
{
	if(pNetUdp->IsRecving())
		return true;

	SOCKET sock = pNetUdp->GetSocket();
	void *pBuffer; int nBufferSize;
	if( !pNetUdp->GetWriteAbleBuffers(pBuffer, nBufferSize) )
		return false;

	if( !AddRecvFrom(sock, pBuffer, nBufferSize) )
		return false;

	pNetUdp->SetRecvStatus(true);
	return true;
}

uint16 IOCPDriver::GetBestThreadNumber()
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);
	int nCpuNum = sysInfo.dwNumberOfProcessors;
	
	return nCpuNum*2+2;
}

bool IOCPDriver::AddMonitor(SOCKET sock)
{
	if(NULL == CreateIoCompletionPort( (HANDLE)sock, m_hCompletionPort, (DWORD)(sock), 0))
	{
		return false;
	}
	return true;
}

bool IOCPDriver::AddAccept(SOCKET listenSock)
{
	if(listenSock == INVALID_SOCKET) 
		return false;

	IOCP_OVERLAPPED *pOverlapped = CreateOverlapped();
	if ( NULL == pOverlapped )
		return false;

	memset( &pOverlapped->_overlapped, 0, sizeof(OVERLAPPED) );
	pOverlapped->_dwLocalAddressLength = sizeof(SOCKADDR_IN) + 16;//客户端局域网IP
	pOverlapped->_dwRemoteAddressLength = sizeof(SOCKADDR_IN) + 16;//客户端外网IP
	memset( pOverlapped->_outPutBuf, 0, sizeof(SOCKADDR_IN)*2+32 );

	Socket client(TCP);
	SOCKET sockClient = client.Dettach();
	pOverlapped->_sock = sockClient;
	pOverlapped->_completiontype = IOS_ACCEPT;
	//投递接受连接操作
	if ( !::AcceptEx( listenSock,
		sockClient,
		pOverlapped->_outPutBuf, 0,
		pOverlapped->_dwLocalAddressLength, 
		pOverlapped->_dwRemoteAddressLength, 
		NULL, &pOverlapped->_overlapped ) )
	{
		int nErrCode = WSAGetLastError();
		if ( ERROR_IO_PENDING != nErrCode ) 
		{
			DeleteOverlapped(pOverlapped);
			return false;
		}
	}

	return true;
}

bool IOCPDriver::AddSend(SOCKET sock, void* buffer, int nDataSize)
{
	IOCP_OVERLAPPED *pOverlapped = CreateOverlapped();
	if ( NULL == pOverlapped ) 
		return false;

	memset( &pOverlapped->_overlapped, 0, sizeof(OVERLAPPED) );
	pOverlapped->_wsaBuffer.buf = (CHAR*)buffer;
	pOverlapped->_wsaBuffer.len = nDataSize;
	pOverlapped->_overlapped.Internal = 0;
	pOverlapped->_sock = sock;
	pOverlapped->_completiontype = IOS_SEND;

	DWORD dwSend = 0;
	DWORD dwFlags = 0;
	//投递数据接收操作
	if ( SOCKET_ERROR == ::WSASend( sock, 
		&pOverlapped->_wsaBuffer,
		1, &dwSend, dwFlags, 
		&pOverlapped->_overlapped, NULL ) )
	{
		int nErrCode = WSAGetLastError();
		if ( ERROR_IO_PENDING != nErrCode ) 
		{
			DeleteOverlapped(pOverlapped);
			pOverlapped = NULL;
			return false;
		}
	}
	return true;
}

bool IOCPDriver::AddRecv(SOCKET sock, void* buffer, int nDataSize)
{
	IOCP_OVERLAPPED *pOverlapped = CreateOverlapped();
	if ( NULL == pOverlapped )
		return false;

	memset( &pOverlapped->_overlapped, 0, sizeof(OVERLAPPED) );
	pOverlapped->_wsaBuffer.buf = (CHAR*)buffer;
	pOverlapped->_wsaBuffer.len = nDataSize;
	pOverlapped->_overlapped.Internal = 0;
	pOverlapped->_sock = sock;
	pOverlapped->_completiontype = IOS_RECV;

	DWORD dwRecv = 0;
	DWORD dwFlags = 0;
	//投递数据接收操作
	if ( SOCKET_ERROR == ::WSARecv( sock, 
		&pOverlapped->_wsaBuffer, 
		1, &dwRecv, &dwFlags, 
		&pOverlapped->_overlapped, NULL ) )
	{
		int nErrCode = WSAGetLastError();
		if ( ERROR_IO_PENDING != nErrCode ) 
		{
			DeleteOverlapped(pOverlapped);
			pOverlapped = NULL;
			return false;
		}
	}
	return true;
}

bool IOCPDriver::AddSendTo(SOCKET sock, void* buffer, int nDataSize, uint64 nAddress)
{
	IOCP_OVERLAPPED *pOverlapped = CreateOverlapped();
	if ( NULL == pOverlapped ) 
		return false;

	memset( &pOverlapped->_overlapped, 0, sizeof(OVERLAPPED) );
	pOverlapped->_wsaBuffer.buf = (char*)buffer;
	pOverlapped->_wsaBuffer.len = nDataSize;
	pOverlapped->_overlapped.Internal = 0;
	pOverlapped->_sock = sock;
	pOverlapped->_completiontype = IOS_SENDTO;

	sockaddr_in sendAddr = Socket::S_GetSockAddrIpV4(nAddress);
	
	DWORD dwSend = 0;
	DWORD dwFlags = 0;
	//投递数据接收操作
	if ( SOCKET_ERROR == ::WSASendTo( sock, 
		&pOverlapped->_wsaBuffer, 
		1, &dwSend, dwFlags, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR),
		&pOverlapped->_overlapped, NULL ))
	{
		int nErrCode = WSAGetLastError();
		if ( ERROR_IO_PENDING != nErrCode ) 
		{
			DeleteOverlapped(pOverlapped);
			pOverlapped = NULL;
			return false;
		}
	}
	return true;
}
bool IOCPDriver::AddRecvFrom(SOCKET sock, void* buffer, int nDataSize)
{
	IOCP_OVERLAPPED *pOverlapped = CreateOverlapped();
	if ( NULL == pOverlapped )
		return false;

	memset( &pOverlapped->_overlapped, 0, sizeof(OVERLAPPED) );
	pOverlapped->_wsaBuffer.buf = (char*)buffer;
	pOverlapped->_wsaBuffer.len = nDataSize;
	pOverlapped->_overlapped.Internal = 0;
	pOverlapped->_sock = sock;
	pOverlapped->_completiontype = IOS_RECVFROM;

	DWORD dwRecv = 0;
	DWORD dwFlags = 0;
	int dwAddrLen = sizeof(sockaddr_in);
	//投递数据接收操作
	if ( SOCKET_ERROR == ::WSARecvFrom(sock,
		&pOverlapped->_wsaBuffer,
		1, &dwRecv, &dwFlags, (SOCKADDR*)&pOverlapped->_clientAddr, &dwAddrLen, 
		&pOverlapped->_overlapped, NULL ) )
	{
		int nErrCode = WSAGetLastError();
		if ( ERROR_IO_PENDING != nErrCode ) 
		{
			DeleteOverlapped(pOverlapped);
			pOverlapped = NULL;
			return false;
		}
	}
	return true;
}
bool IOCPDriver::WaitEvent(IO_EVENT &event)
{
	/*
		完成操作上的传输长度
		发现accept操作完成时为0
		发现recv/send操作完成时，记录接收/发送字节数
		如果recv/send操作上的传输长度小于等于0表示套接字已关闭
	*/
	DWORD dwIOSize;
	IOCP_OVERLAPPED *pOverlapped = NULL;//接收完成数据
	memset(&event, 0, sizeof(event));
	event._nIOType = IOS_ERROR;

	if ( !::GetQueuedCompletionStatus( m_hCompletionPort, &dwIOSize, 
						(LPDWORD)&event._sock, (OVERLAPPED**)&pOverlapped,
						INFINITE ) )
	{
		DWORD dwErrorCode = ::GetLastError();
		if ( WAIT_TIMEOUT == dwErrorCode ||  // 超时
			ERROR_INVALID_HANDLE == dwErrorCode ) //启动时，未知原因非法句柄，可能是捕获到前一次程序关闭后未处理的完成事件
		{
			try
			{
				if ( NULL != pOverlapped )
				{
					DeleteOverlapped(pOverlapped);
					return true;
				}
			}catch(...)
			{
			}
			return true;
		}
		if ( ERROR_OPERATION_ABORTED == dwErrorCode ) //Accept用于接收新连接的socket关闭
		{
			if ( IOS_ACCEPT == pOverlapped->_completiontype ) 
			{
				DeleteOverlapped(pOverlapped);
				return true;
			}
		}
		if ( IOS_ACCEPT == pOverlapped->_completiontype ) //Accept上的socket关闭，重新投递监听
		{
			AddAccept(event._sock);
		}
		else if( IOS_RECVFROM == pOverlapped->_completiontype ) //RecvFrom 上的socket关闭，心跳检查关闭
		{
			event._client = pOverlapped->_sock;
			return true;
		}
		else//客户端异常断开，拔网线，断电，终止进程
		{
			event._nIOType = IOS_DISCONNECT;
			event._client = pOverlapped->_sock;
		}
	}
	else if ( IOS_CLOSE == pOverlapped->_completiontype ) //用户Stop
	{
		event._nIOType = IOS_CLOSE;
		event._client = pOverlapped->_sock;
	}
	else if ( 0 == dwIOSize && IOS_RECV== pOverlapped->_completiontype )
	{
		event._nIOType = IOS_DISCONNECT;
		event._client = pOverlapped->_sock;
	}
	else//io事件
	{
		//io事件
		event._nIOType = pOverlapped->_completiontype;
		event._client = pOverlapped->_sock;
		event._pBuffer = pOverlapped->_wsaBuffer.buf;
		event._nDataSize = dwIOSize;
		event._udpClientAddress	= pOverlapped->_clientAddr;
	}
	DeleteOverlapped(pOverlapped);
	return true;
}

IOCP_OVERLAPPED* IOCPDriver::CreateOverlapped()
{
	return new ((IOCP_OVERLAPPED*)MemoryPoolManager::Single()->GetMemoryPool(MPID_SOCK)->Alloc(sizeof(IOCP_OVERLAPPED))) IOCP_OVERLAPPED;
}

void IOCPDriver::DeleteOverlapped(IOCP_OVERLAPPED* &pDel)
{
	MemoryPoolManager::Single()->GetMemoryPool(MPID_SOCK)->Free(pDel);
	pDel = nullptr;
}

};//namespace XZBEN
