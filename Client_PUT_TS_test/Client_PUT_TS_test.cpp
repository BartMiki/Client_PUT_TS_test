// Client_PUT_TS_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") 

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

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set connection socket
	if (connect(Connection, (SOCKADDR*)&addr, addrLen) != 0) //If connecting to server failed
	{
		MessageBoxA(NULL, "Failed to connect", "Error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE; //Failed to connect to server
	}
	//Succesful connection to server
	std::cout << "Connected to server!" << std::endl;
	char messege[128];
	recv(Connection, messege, sizeof(messege), NULL); //Recieved message
	std::cout << "Message: " << messege << std::endl;
	getchar();
	return EXIT_SUCCESS;
}
