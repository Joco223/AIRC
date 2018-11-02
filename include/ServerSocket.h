#pragma once

#include <SDL_net.h>
#include <string>

#include "MessageList.h"

class ServerSocket {
private:
	unsigned int port;
	unsigned int bufferSize;
	unsigned int maxSockets;
	unsigned int maxClients;

	IPaddress serverIP;
	TCPsocket serverSocket;	

	TCPsocket* pClientSocket;
	bool* pSocketIsFree;
	std::string* clientNames;
	char* pBuffer;

	SDLNet_SocketSet socketSet;
	unsigned int clientCount;
public:
	std::string dotQuadString;
	ServerSocket(unsigned int, unsigned int, unsigned int);

	void checkForConnections(std::string, std::string, MessageList&);
	void sendMessages(std::string, std::string);
	void sendWritingStatus(std::string, std::string);
	int checkForActivity(std::string&, MessageList&);
	void dealWithActivity(unsigned int, std::string&, std::string&);
};
