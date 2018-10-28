#pragma once

#include <SDL_net.h>
#include <string>

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

	void checkForConnections();
	void sendMessages(std::string);
	int checkForActivity();
	void dealWithActivity(unsigned int, std::string&);

};
