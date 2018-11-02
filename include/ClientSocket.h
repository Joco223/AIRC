#pragma once

#include <SDL_net.h>
#include <string>
#include <iostream>

#include "MessageList.h"

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

	void connectToServer(std::string, MessageList&);
	void sendMessage(std::string);
	void sendWritingStatus(std::string, std::string);
	std::string checkForIncomingMessages(std::string&, std::string&, MessageList&, bool&);
};