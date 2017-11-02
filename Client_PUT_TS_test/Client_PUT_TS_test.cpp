// Client_PUT_TS_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") 

// --- Setup ---

SOCKET connection;

void clientThread()
{
	int bufferSize;

	while (true)
	{
		recv(connection, (char*)&bufferSize, sizeof(int), NULL); //Receive lenght of message
		char * buffer = new char[bufferSize + 1]; //Buffer fo upcomming message
		buffer[bufferSize] = '\0';
		recv(connection, buffer, bufferSize + 1, NULL); //Receive message from sever
		std::cout << ">" <<buffer << std::endl;
		delete[] buffer;
	}
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
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientThread, NULL, NULL, NULL);

	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer); //User input
		buffer += '\0';
		int bufferSize = buffer.size();
		send(connection, (char*)&bufferSize, sizeof(int), NULL); //Sending lenght of user message
		send(connection, buffer.c_str(), bufferSize, NULL); //Sending user message to server
		Sleep(10);
	}

	getchar();
	return EXIT_SUCCESS;
}