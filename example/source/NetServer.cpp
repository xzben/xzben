#include <Socket/NetHost.h>
#include <Socket/IOCPDriver.h>
#include <Socket/NetProtocol.h>
#include <base/Lock.h>
#include <string>
using namespace XZBEN;

class TestProtocol : public NetProtocol
{
private:
	bool OnConnect(NetSocket *pNetSocket)
	{
		std::string strIp; uint16 nPort;
		pNetSocket->GetPeertAddr(strIp, nPort);

		Print(GlobalFunction::Format("Connect Ip: %s --- Port: %d", strIp.c_str(), nPort));
		return true;
	}
	bool OnDisconnect(NetSocket *pNetSocket)
	{
		std::string strIp; uint16 nPort;
		pNetSocket->GetPeertAddr(strIp, nPort);

		Print(GlobalFunction::Format("Disconnect Ip: %s --- Port: %d", strIp.c_str(), nPort));
		return true;
	}
	bool OnMsg(NetSocket *pNetSocket)
	{
		char buffer[1024];
		static int nTimes = 1;
		int nDataSize = pNetSocket->GetDataSize();
		int nReadSize;
		if( (nReadSize = pNetSocket->ReadMsg(buffer, 500)) > 0 )
		{
			buffer[nReadSize] = '\0';
			Print(GlobalFunction::Format("%d: OnMsg DataSize :%d Content: %s",nTimes++, nDataSize, buffer));
			pNetSocket->SendMessage(buffer, nReadSize);
		}
		
		return true;
	}
private:
	void Print(std::string strMsg)
	{
		AutoLock lock(&m_mutex);
		printf("%s\n", strMsg.c_str());
	}
	Mutex m_mutex;
};
int main()
{
	NetHost *pHost = new NetHost(new IOCPDriver, new TestProtocol);
	pHost->StartServer();
	pHost->ListernUdpPort(6001);
	pHost->ThreadLoop();
	return 0;
}