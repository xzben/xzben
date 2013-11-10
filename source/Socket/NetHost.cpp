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

#include "NetHost.h"
#include "IODriver.h"
#include "NetProtocol.h"
#include "NetSocket.h"

namespace XZBEN
{
NetHost::NetHost(IODriver *pDriver /* = nullptr */, NetProtocol *pProtocl /* = nullptr */)
	:m_pDriver(nullptr), m_pProtocl(nullptr)
{
	SetupDriver(pDriver);
	SetupProtocol(pProtocl);
	m_bStart = false;
}
NetHost::~NetHost()
{
	StopServer();
	SAFE_DELETE(m_pProtocl);
	SAFE_DELETE(m_pDriver);
}
bool NetHost::SetupDriver(IODriver *pDriver)
{
	if(nullptr == pDriver)
		return false;
	if(nullptr != m_pDriver) //已经装载过
		return false;

	m_pDriver = pDriver;

	m_pDriver->Attach(this); //Host 与 Protocol 监视IODriver
	if(nullptr != m_pProtocl)
		m_pDriver->Attach(m_pProtocl);

	return true;
}

bool NetHost::SetupProtocol(NetProtocol *pProtocol)
{
	if(nullptr == pProtocol)
		return false;
	if(nullptr != m_pProtocl)
		return false;

	m_pProtocl = pProtocol;
	if(nullptr != m_pDriver)
		m_pDriver->Attach(m_pProtocl);

	return true;
}

bool NetHost::StartServer(int nThreadNum /*= -1*/, bool bHeartCheck /*= false*/, int16 nHeartGap /*= 10*/)
{
	if( !IsReady() )
		return false;
	return m_bStart = m_pDriver->Start(nThreadNum, bHeartCheck, nHeartGap);
}

bool NetHost::StopServer()
{
	if( !IsReady() && !m_bStart)
		return false;

	m_pDriver->Stop();
	m_bStart = false;
	return true;
}

void NetHost::ThreadLoop()
{
	if( !IsReady() || !m_bStart)
		return;

	m_pDriver->ThreadLoop();
}

bool NetHost::ListernTcpPort(uint16 nPort)
{
	if( !IsReady() && !m_bStart)
		return false;

	return m_pDriver->ListernTcpPort(nPort);
}

bool NetHost::ListernUdpPort(uint16 nPort)
{
	if(!IsReady() && !m_bStart)
		return false;

	return m_pDriver->ListernUdpPort(nPort);
}

SOCKET	NetHost::Connect(const char* szIp, uint16 nPort, bool bAutoConnect /*= false*/)
{
	if( !IsReady()  && !m_bStart)
		return INVALID_SOCKET;

	return m_pDriver->Connect(szIp, nPort, bAutoConnect);
}

bool	NetHost::SendTcpMsg(SOCKET hSock, void* pBuffer, int nDataSize)
{
	if( !IsReady()  && !m_bStart)
		return false;

	return m_pDriver->SendTcpMSG(hSock, pBuffer, nDataSize);
}

bool NetHost::SendUdpMSG(const char* szIp, uint16 nPort, void* pBuffer, int nLen)
{
	if( !IsReady()  && !m_bStart)
		return false;

	return m_pDriver->SendUdpMSG(szIp, nPort, pBuffer, nLen);
}

bool NetHost::IsReady()
{
	return nullptr != m_pDriver && nullptr != m_pProtocl;
}

bool NetHost::Update(STATUS status, NetSocket* pNetSocket)
{
	return true;
}
};//namespace XZBEN
