#pragma once

#include <SDL_net.h>
#include <string>
#include <iostream>

class ClientSocket {
private:
	int serverPort;
	int bufferSize;

	std::string serverHostName;
	IPaddress serverIP;
	TCPsocket serverSocket;
	TCPsocket clientSocket;

	char* pBuffer;
	SDLNet_SocketSet socketSet;
public:

	ClientSocket(std::string, int, int);

	void connectToServer(std::string);
	void sendMessage(std::string);
	std::string checkForIncomingMessages();
};