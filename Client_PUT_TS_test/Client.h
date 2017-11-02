#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") 

enum struct Packet : char {
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
	CloseFail = (char)0x3c//			fail | close
};

class Client
{
public:
	// --- methods ---
	Client(std::string IP, int PORT);
	bool Connect();
	bool CloseConnection();

	// --- method send---
	bool SendString(std::string str, Packet packetType = Packet::MsgSend);
private:
	// --- data fields ---
	SOCKET connection;
	SOCKADDR_IN addr; //Addres to bind connection socket to
	int sizeOfAddr;

	// --- methods send---
	bool SendPacketType(Packet packetType);
	bool SendInt64(u_int64 num);

	// --- methods recv ---
	bool RecvPacketType(Packet & packetType);
	bool RecvInt64(u_int64 & num);
	bool RecvString(std::string & str);

	// --- methods others--
	bool ProcessPacket(Packet packetType);
	static void ClientThread();
};

static Client * clientPtr;