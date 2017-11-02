// Client_PUT_TS_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") 

// --- Setup ---

enum struct Packet : char{
	//								  reply  | operation
	//								---------+----------
	InviteSend = (char)0x1f,//			none | invite
	InviteAccepted = (char)0x2f,//	  accept | invite
	InviteDecline = (char)0x3f,//	 decline | invite
	MsgSend = (char)0x1e,//				none | sendMsg
	MsgReceived = (char)0x2e,//		recieved | sendMsg
	MsgFailure = (char)0x3e,//		  failed | sendMsg
	MsgServer = (char)0x4e,//			none | sendMsg
	CloseSession = (char)0x1c,//		none | close
	CloseSucces = (char)0x2c,//		 success | close
	CloseFail =  (char)0x3c//			fail | close
};

SOCKET connection;

bool SendInt64(u_int64 num)
{
	int check = send(connection, (char*)&num, sizeof(u_int64), NULL); //Send num in 8 bytes
	if (check == SOCKET_ERROR) //If sending failed
		return false;
	return true;
}

bool RecvInt64(u_int64 & num)
{
	int check = recv(connection, (char*)&num, sizeof(u_int64), NULL); //Receive 8 bytes (64 bits) num
	if (check == SOCKET_ERROR) //If receiving failed
		return false;
	return true;
}

bool SendPacketType(Packet packetType)
{
	int check = send(connection, (char*)&packetType, sizeof(Packet), NULL); //Send packetType in 1 byte
	if (check == SOCKET_ERROR) //If sending failed
		return false;
	return true;
}

bool RecvPacketType(Packet & packetType)
{
	int check = recv(connection, (char*)&packetType, sizeof(Packet), NULL); //Receive 1 byte (8 bit) packetType
	if (check == SOCKET_ERROR) //If receiving failed
		return false;
	return true;
}

bool SendString(std::string str, Packet packetType = Packet::MsgSend)
{
	if (!SendPacketType(packetType)) //If failed to send packet header
		return false;

	//str += "\0";
	u_int64 strSize = str.size();
	if (!SendInt64(strSize)) //If failed to send size of string
		return false;

	int check = send(connection, str.c_str(), strSize, NULL); //Send string in strSize bytes
	if (check == SOCKET_ERROR) //If sending failed
		return false;
	return true;
}

bool RecvString(std::string & str)
{
	u_int64 bufferSize;
	if (!RecvInt64(bufferSize)) //If failed to receive size of upcomming string
		return false;

	char * buffer = new char[bufferSize + 1];
	buffer[bufferSize] = '\0'; //adding terminator

	int check = recv(connection, buffer, bufferSize, NULL); //recv string in bufferSize bytes
	str = buffer;
	delete[] buffer;
	if (check == SOCKET_ERROR) //If receiving failed
		return false;
	return true;
}

bool ProcessPacket(Packet packetType) 
{
	switch (packetType)
	{
	case Packet::MsgServer:
	{
		std::string message;
		if (!RecvString(message))
			return false;
		std::cout << "# " << message << " #"<< std::endl;
		break;
	}
	case Packet::MsgSend:
	{
		std::string message;
		if (!RecvString(message))
			return false;
		std::cout << ">" << message << std::endl;
		Packet packetRecv = Packet::MsgReceived; //Message received properly
		if (!SendPacketType(packetRecv)) //If there is error in returnin message
			return false;
		break;
	}
	case Packet::MsgReceived:
		std::cout << "# Message delivered #" << std::endl;
		break;
	default:
		std::cout << "Unrecognized packet: " << std::hex << (int)packetType << std::endl;
		break;
	}
	return true;
}

void ClientThread()
{
	Packet packetType;
	while (true)
	{
		if (!RecvPacketType(packetType)) //If appeard issue with receiving packetType
			break; //Break infinite loop
		if (!ProcessPacket(packetType)) //If packet is not properly processed
			break; //Break infinite loop
	}
	std::cout << "# Lost connection to the server. #" << std::endl;
	closesocket(connection); //Close socked used by client after error occured 
}

int main()
{
	//WinSock2 startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 2);

	//if WSAStartup return anything other than 0, that means an error has occured
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	// --- Address setup ---

	SOCKADDR_IN addr; //Addres to bind connection socket to
	int addrLen = sizeof(addr); //Addres lenght
	inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr)); //Addres = local host (this PC)
	addr.sin_port = htons(5555); //Port ("htons" means "Host TO Network Short")
	addr.sin_family = AF_INET; //IPv4 Socket
								   
	// --- Connection socket ---

	connection = socket(AF_INET, SOCK_STREAM, NULL); //Set connection socket
	if (connect(connection, (SOCKADDR*)&addr, addrLen) != 0) //If connecting to server failed
	{
		MessageBoxA(NULL, "Failed to connect", "Error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE; //Failed to connect to server
	}
	//Succesful connection to server
	std::cout << "Connected to server!" << std::endl;

	//Create client thread to handle upcomming messages
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);

	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer); //User input
		//buffer += '\0';
		if (!SendString(buffer))
			break;
		Sleep(10);
	}

	getchar();
	return EXIT_SUCCESS;
}