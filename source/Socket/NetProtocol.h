/************************************************************************************
 *	File   Name: NetProtocol.h														*
 * 	Description:																	*
 *																					*
 *	Create Time:																	*
 *		 Author: xzben																*
 *	Author Blog:  www.xzben.com														*
 ************************************************************************************
 * Modify Log: 																		*
 *																					*
 ************************************************************************************/

#ifndef __2013_10_27_NETPROTOCOL_H__
#define __2013_10_27_NETPROTOCOL_H__

#include "Observer.h"
#include "NetSocket.h"

namespace XZBEN
{
class NetSocket;
class NetTcp;
class NetUdp;

/*
*	NetProtocol:	网络通信协议层抽象接口层，通过定义具体的子类来实现具体的通信协议，协议靠类中OnConnect、OnDisconnect、OnMsg接口实现
*	使用步骤：
*		1、定义一个子类继承于NetProtocol
*		2、实现接口OnConnect、OnDisconnect、OnMsg，这里接口的实现按照通信协议需要实现
*		3、实例化一个具体子类传递给NetHost对象，告诉NetHost通信该如何处理IO数据
*/
class NetProtocol : public Observer
{
public:
	NetProtocol();
	virtual ~NetProtocol();
	/*
	*	OnConnect接口在一个 TCP 连接建立时触发，可以在此接口中实现对连接到网络主机的Socket进行信息采集
	*	@para pNetSocket:	一个连接到主机的Socket连接封装类。可以在其中获取连接的通信数据，连接对象的地址信息也可以用它直接给通信对法发送数据。
	*	
	*/
	virtual	bool OnConnect(ShareNetSocketPtr& pNetSocket) =0;
	/*
	*	OnDisconnect接口在一个TCP连接断开时触发，可以在此接口中采集与主机断开连接的的socket信息
	*	@para pNetSocket: 同OnConnect
	*/
	virtual bool OnDisconnect(ShareNetSocketPtr& pNetSocket)=0;
	/*
	*	OnMsg 接口在接收到一个TCP/UDP 数据包是触发，要注意的是如果是TCP这里的触发次数不一定和发送端发送次数一致。
	*		所以你要自己通过数据内容中的协议判断数据是否发送结束，不能假设一定一次触发就是一个完整的数据包。
	*	@para pNetSocket: 同OnConnect	
	*/
	virtual bool OnMsg(ShareNetSocketPtr& pNetSocket) =0;
protected:
	//virtual
	/*
	*	
	*/
	bool Update(STATUS status, ShareNetSocketPtr& pNetSocket)override;
};

}//namespace XZBEN
#endif//__YYYY_MM_DD_XXXXX_H__