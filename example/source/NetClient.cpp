#include <Socket/NetHost.h>
#include <Socket/IOCPDriver.h>
#include <Socket/NetProtocol.h>
using namespace XZBEN;


class ClientProtocol : public NetProtocol
{
private:
	bool OnConnect(ShareNetSocketPtr& pNetSocket)
	{
		std::string strIp; uint16 nPort;
		pNetSocket->GetPeertAddr(strIp, nPort);

		Print(GlobalFunction::Format("Connect Ip: %s --- Port: %d", strIp.c_str(), nPort));
		return true;
	}
	bool OnDisconnect(ShareNetSocketPtr& pNetSocket)
	{
		std::string strIp; uint16 nPort;
		pNetSocket->GetPeertAddr(strIp, nPort);

		Print(GlobalFunction::Format("Disconnect Ip: %s --- Port: %d", strIp.c_str(), nPort));
		return true;
	}
	bool OnMsg(ShareNetSocketPtr& pNetSocket)
	{
		char buffer[1024];
		static int nTimes = 1;
		int nDataSize = pNetSocket->GetDataSize();
		int nReadSize;
		if( (nReadSize = pNetSocket->ReadMsg(buffer, 500)) > 0 )
		{
			buffer[nReadSize] = '\0';
			Print(GlobalFunction::Format("%d: OnMsg DataSize :%d Content: %s",nTimes++, nDataSize, buffer));
			pNetSocket->SendMessage("TestBuffer", strlen("TestBuffer")+1);
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
	NetHost host;
	host.SetupDriver(new IOCPDriver);
	host.SetupProtocol(new ClientProtocol);
	host.StartServer(1);
	///* TCP
	SOCKET hSocket = host.Connect("127.0.0.1", 6000);
	host.SendTcpMsg(hSocket, "TestBuffer", strlen("TestBuffer")+1);
	//*/
	
	/*UDP
	host.SendUdpMSG("127.0.0.1",6001, "TestBuffer", strlen("TestBuffer")+1);
	//*/
	host.ThreadLoop();
	return 0;
}