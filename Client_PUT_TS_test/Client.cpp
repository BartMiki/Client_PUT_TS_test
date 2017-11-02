#include "stdafx.h"
#include "Client.h"

Client::Client(std::string IP, u_short PORT)
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

	inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr)); //Addres
	addr.sin_port = htons(PORT); //Port ("htons" means "Host TO Network Short")
	addr.sin_family = AF_INET; //IPv4 Socket
}

bool Client::Connect()
{
	// --- Connection socket ---
	connection = socket(AF_INET, SOCK_STREAM, NULL); //Set connection socket
	if (connect(connection, (SOCKADDR*)&addr, sizeOfAddr) != 0) //If connecting to server failed
	{
		MessageBoxA(NULL, "Failed to connect", "Error", MB_OK | MB_ICONERROR);
		return false; //Failed to connect to server
	}
	//Succesful connection to server
	return true;
}

bool Client::CloseConnection()
{
	return false;
}

bool Client::SendPacket(std::string & str)
{

	return false;
}

bool Client::ProcessPacket()
{
	std::string message;
	if (!RecvString(message))
		return false;
	std::cout << ">" << message << std::endl;
	return true;
}

void Client::ClientThread()
{
	int bufferSize;

	while (true)
	{
		recv(connection, (char*)&bufferSize, sizeof(int), NULL); //Receive lenght of message
		char * buffer = new char[bufferSize + 1]; //Buffer fo upcomming message
		buffer[bufferSize] = '\0';
		recv(connection, buffer, bufferSize + 1, NULL); //Receive message from sever
		std::cout << ">" << buffer << std::endl;
		delete[] buffer;
	}
}

bool Client::RecvPacket(std::string & str)
{
	return false;
}

