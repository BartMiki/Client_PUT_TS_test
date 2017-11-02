#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") 

class Client
{
public:
	Client(std::string IP, u_short PORT);
	bool Connect();
	bool CloseConnection();
	bool SendPacket(std::string & str);
private:

	bool ProcessPacket();
	static void ClientThread();
	bool RecvPacket(std::string & str);

	SOCKET connection;
	SOCKADDR_IN addr; //Addres to bind connection socket to
	int sizeOfAddr = sizeof(addr);
};

