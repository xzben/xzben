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

#include "NetProtocol.h"

namespace XZBEN
{

NetProtocol::NetProtocol()
{

}

NetProtocol::~NetProtocol()
{

}

bool NetProtocol::Update(STATUS status, NetSocket *pNetSocket)
{
	switch(status)
	{
	case IOS_DISCONNECT:
		OnDisconnect(pNetSocket);
		break;
	case IOS_ACCEPT:
		OnConnect(pNetSocket);
		break;
	case IOS_RECV:
	case IOS_RECVFROM:
		OnMsg(pNetSocket);
		break;
	}
	return true;
}
};//namespace XZBEN
