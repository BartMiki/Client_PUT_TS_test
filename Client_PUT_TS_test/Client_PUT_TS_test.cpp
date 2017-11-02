// Client_PUT_TS_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"

int main()
{
	
	Client myClient("127.0.0.1", 5555); //Create client to local host 127.0.0.1 to port 5555

	if (!myClient.Connect())
	{
		std::cout << "Failed to connect to server." << std::endl;
		getchar();
		return EXIT_FAILURE;
	}

	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer); //User input
		if (!myClient.SendString(buffer))
			break;
		Sleep(10);
	}

	getchar();
	return EXIT_SUCCESS;
}