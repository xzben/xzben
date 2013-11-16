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

#ifndef __2013_10_27_NETCONNECT_H__
#define __2013_10_27_NETCONNECT_H__

#include "Socket.h"
#include "IODriver.h"
#include "../Base/AllocFromMemoryPool.h"
#include "../Base/IOBuffer.h"
#include "../Base/SharePtr.h"
#include <ctime>

namespace XZBEN
{
class NetSocket : public AllocFromMemoryPool<MPID_SOCK>
{
public:
	friend class IOCPDriver;
	friend class IODriver;
	friend class NetSocketPool;
	friend class NetSocketUseHelper;
	
	NetSocket(IODriver *pDriver, bool bHeartCheck);
	NetSocket(IODriver *pDriver, EProtocl epType, bool bHeartCheck);
	virtual ~NetSocket();

	bool	IsSending();
	bool	IsRecving();
	bool	IsOperating();
	SOCKET	GetSocket();
	time_t	GetLastHeart();
	void	RefreshHeart();
	bool GetLocalAddr(std::string& strIp, uint16& nPort);
	bool GetPeertAddr(std::string& strIp, uint16& nPort);
	bool GetSendBuffer(void *&pBuffer, int &nLen);
	int	 GetDataSize();
	int	 ReadMsg(void *pBuffer, int nDataSize, bool bDel = true);
	virtual int			GetID() { return GetSocket(); };
	virtual EProtocl	GetType() = 0;
	virtual bool		SendMessage(void *pBuffer, int nDataSize) = 0;
	
protected:
	bool IsReady(); 
	bool SetReady(bool bReady);
	bool	NeedHeartCheck();
	bool	AttachSocket(SOCKET sock, EProtocl epType);
	bool	SetRecvStatus(bool bRecving);
	bool	SetSendStatus(bool bSending);
	bool	SetOperatorStatus(bool bOperating);
	virtual bool SendOut(int nLen) = 0;
	virtual	bool RecvDone(int nLen) = 0;
	virtual bool GetWriteAbleBuffers(void *&pBuffer, int &nBufferSize) = 0;
protected:
	bool				 m_bIsSending;
	bool				 m_bIsRecving;
	bool				 m_bHeartCheck;
	bool				 m_bReady;
	bool				 m_bIsOperating;
	Socket				 m_socket;
	IODriver			*m_pDriver;
	Mutex				 m_mutex;
	time_t				m_tmLastHeart;
	IOBuffer<MPID_SOCK>	m_SendBuffer;
	IOBuffer<MPID_SOCK>	m_RecvBuffer;
};

class NetTcp : public NetSocket
{
public:
	friend class IODriver;
	friend class NetHost;
	friend class NetSocketPool;

	NetTcp(IODriver *pDriver, bool bAutoConnect = false, bool bHeartCheck = false);
	virtual ~NetTcp();
	EProtocl GetType() override{ return TCP; }
	bool Connect(const char* szIp, uint16 nPort);
	bool SendMessage(void *pBuffer, int nDataSize)override;
	
protected:
	bool CreateConnect(SOCKET hSocket);
	bool Reconnect();
	bool NeedAutoReConnect();
	bool GetWriteAbleBuffers(void *&pBuffer, int &nBufferSize)override;
	bool SendOut(int nLen)override;
	bool RecvDone(int nLen)override;
private:
	bool	m_bAutoConnect;
			
};

class NetUdp : public NetSocket
{
public:
	friend class IODriver;
	friend class NetHost;
	friend class NetSocketPool;
	friend class NetServerUdp;

	NetUdp(IODriver *pDriver, bool bHeartCheck = false);
	NetUdp(IODriver *pDriver, SOCKET sock, bool bHeartCheck = false);
	virtual ~NetUdp();

	EProtocl GetType() override { return UDP; }
	bool SendMessage(void *pBuffer, int nDataSize)override;
protected:
	bool SetPeer(const char* szIp, uint16 nPort);
	bool SetPeer(sockaddr_in Peeraddr);

	bool RecvDone(int nLen)override;
	bool SendOut(int nLen)override;
	bool GetWriteAbleBuffers(void *&pBuffer, int &nBufferSize)override;
};

class NetServerUdp : public NetUdp
{
public:
	NetServerUdp(IODriver *pDriver, SOCKET	sock);
	virtual ~NetServerUdp();
protected:
	bool RecvDone(int nLen)override;
	bool SendOut(int nLen)override;
};

typedef SharePtr<NetSocket>		ShareNetSocketPtr;
typedef SharePtr<NetTcp>		ShareNetTcpPtr;
typedef SharePtr<NetUdp>		ShareNetUdpPtr;
#define DELETE_SHARE_PTR(_sharePtr)	do { _sharePtr = nullptr;} while(0)

}//namespace XZBEN
#endif//__2013_10_27_NETCONNECT_H__