#include "stdafx.h"
#include "Client.h"

Client::Client(std::string IP, int PORT)
{
	//WinSock2 startup
	DllVersion = MAKEWORD(2, 2);

	//if WSAStartup return anything other than 0, that means an error has occured
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}
	// --- Address setup ---
	sizeOfAddr = sizeof(addr);
	inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr)); //Addres = local host (this PC)
	addr.sin_port = htons(PORT); //Port ("htons" means "Host TO Network Short")
	addr.sin_family = AF_INET; //IPv4 Socket

	clientPtr = this;
}

bool Client::Connect()
{
	// --- Connection socket ---
	connection = socket(AF_INET, SOCK_STREAM, NULL); //Set connection socket
	int code = connect(connection, (SOCKADDR*)&addr, sizeof(addr));
	if (code != 0) //If connecting to server failed
	{
		code = WSAGetLastError();
		std::string error = "Failed to connect, code: " + std::to_string(code);
		MessageBoxA(NULL, error.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false; //Failed to connect to server
	}

	//Succesful connection to server
	std::cout << "Connected to server!" << std::endl;

	//Create client thread to handle upcomming messages
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);
	return true;
}

bool Client::CloseConnection()
{
	if (closesocket(connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK) //If socket error occured
			return true; //Connection has been closed

		std::string error = "Failed to close Socket, WinSock: "+std::to_string(WSAGetLastError())+".";
		MessageBoxA(NULL, error.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false; //Failed to close connection
	}
	return true;
}

bool Client::SendString(std::string str, Packet packetType)
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

bool Client::SendPacketType(Packet packetType)
{
	int check = send(connection, (char*)&packetType, sizeof(Packet), NULL); //Send packetType in 1 byte
	if (check == SOCKET_ERROR) //If sending failed
		return false;
	return true;
}

bool Client::SendInt64(u_int64 num)
{
	int check = send(connection, (char*)&num, sizeof(u_int64), NULL); //Send num in 8 bytes
	if (check == SOCKET_ERROR) //If sending failed
		return false;
	return true;
}

bool Client::RecvPacketType(Packet & packetType)
{
	int check = recv(connection, (char*)&packetType, sizeof(Packet), NULL); //Receive 1 byte (8 bit) packetType
	if (check == SOCKET_ERROR) //If receiving failed
		return false;
	return true;
}

bool Client::RecvInt64(u_int64 & num)
{
	int check = recv(connection, (char*)&num, sizeof(u_int64), NULL); //Receive 8 bytes (64 bits) num
	if (check == SOCKET_ERROR) //If receiving failed
		return false;
	return true;
}


bool Client::RecvString(std::string & str)
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

bool Client::ProcessPacket(Packet packetType)
{
	switch (packetType)
	{
	case Packet::MsgServer:
	{
		std::string message;
		if (!RecvString(message))
			return false;
		std::cout << "# " << message << " #" << std::endl;
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

void Client::ClientThread()
{
	Packet packetType;
	while (true)
	{
		if (!clientPtr->RecvPacketType(packetType)) //If appeard issue with receiving packetType
			break; //Break infinite loop
		if (!clientPtr->ProcessPacket(packetType)) //If packet is not properly processed
			break; //Break infinite loop
	}
	std::cout << "# Lost connection to the server. #" << std::endl;
	if (clientPtr->CloseConnection()) {
		std::cout << "# Socked to the server was closed succesfuly. #" << std::endl;
	}
	else
	{
		std::cout << "# Socked was not able to be closed. #" << std::endl;
	}
}