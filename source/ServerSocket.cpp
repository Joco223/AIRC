#include "ServerSocket.h"
#include <iostream>

ServerSocket::ServerSocket(unsigned int port_, unsigned int bufferSize_, unsigned int maxSockets_) 
			:
			port(port_),
			bufferSize(bufferSize_),
			maxSockets(maxSockets_),
			maxClients(maxSockets_ - 1) {
				pClientSocket = new TCPsocket[maxClients];
				pSocketIsFree = new bool[maxClients];
				pBuffer = new char[bufferSize];
				clientNames = new std::string[maxClients];
				clientCount = 0;

				socketSet = SDLNet_AllocSocketSet(maxSockets);
				for(int i = 0; i < maxClients; i++) {pClientSocket[i] = nullptr; pSocketIsFree[i] = true;}
				SDLNet_ResolveHost(&serverIP, nullptr, port_);

				unsigned char* dotQuad = (unsigned char*)&serverIP.host;

				dotQuadString  = std::to_string((unsigned char)dotQuad[0]);
				dotQuadString += ".";
				dotQuadString += std::to_string((unsigned char)dotQuad[1]);
				dotQuadString += ".";
				dotQuadString += std::to_string((unsigned char)dotQuad[2]);
				dotQuadString += ".";
				dotQuadString += std::to_string((unsigned char)dotQuad[3]);

				serverSocket = SDLNet_TCP_Open(&serverIP);
				SDLNet_TCP_AddSocket(socketSet, serverSocket);
			};

void ServerSocket::checkForConnections(std::string userName, MessageList& userList) {
	int numActiveSockets = SDLNet_CheckSockets(socketSet, 10);
	int serverSocketActivity = SDLNet_SocketReady(serverSocket);
	if(serverSocketActivity != 0) {
		int freeSpot = -99;
		if(clientCount < maxClients) {	
			for(int i = 0; i < maxClients; i++) {
				if(pSocketIsFree[i] == true) {
					pSocketIsFree[i] = false;
					freeSpot = i;
					break;
				}
			}
			pClientSocket[freeSpot] = SDLNet_TCP_Accept(serverSocket);
			SDLNet_TCP_AddSocket(socketSet, pClientSocket[freeSpot]);
			clientCount++;
			strcpy(pBuffer, "OK");
			int msgLength = 3;
			SDLNet_TCP_Send(pClientSocket[freeSpot], (void*)pBuffer, msgLength);
			int receivedByteCount = SDLNet_TCP_Recv(pClientSocket[freeSpot], pBuffer, 16);
			clientNames[freeSpot] = pBuffer;
			userList.addMessage("", "", clientNames[freeSpot]);
			std::string empty = "";

			for(int i = 0; i < maxClients; i++) {
				if((i != freeSpot) && (pSocketIsFree[i] == false)) {
					SDLNet_TCP_Send(pClientSocket[i], (void*)(clientNames[freeSpot].c_str()), 16);
					SDLNet_TCP_Send(pClientSocket[i], (void*)(empty.c_str()), 512);
				}
			}

			for(int i = 0; i < maxClients; i++) {
				if((i != freeSpot) && (pSocketIsFree[i] == false)) {
					SDLNet_TCP_Send(pClientSocket[freeSpot], (void*)(clientNames[i].c_str()), 16);
				}
			}

			std::string end = "END";

			SDLNet_TCP_Send(pClientSocket[freeSpot], (void*)(userName.c_str()), 16);
			SDLNet_TCP_Send(pClientSocket[freeSpot], (void*)(end.c_str()), 16);
		}else{
			TCPsocket tempSock = SDLNet_TCP_Accept(serverSocket);
			strcpy(pBuffer, "FULL");
			int msgLength = 5;
			SDLNet_TCP_Send(pClientSocket[freeSpot], (void*)pBuffer, msgLength);
			SDLNet_TCP_Close(tempSock);
		}
	}
}

void ServerSocket::dealWithActivity(unsigned int clientNumber, std::string& userName, std::string& message) {
	userName = clientNames[clientNumber];
	for(int i = 0; i < maxClients; i++) {
		if((message.length() > 1) && (i != clientNumber) && (pSocketIsFree[i] == false)) {
			SDLNet_TCP_Send(pClientSocket[i], (void*)(clientNames[i].c_str()), 16);
			SDLNet_TCP_Send(pClientSocket[i], (void*)(message.c_str()), 512);
		}
	}
}

void ServerSocket::sendMessages(std::string user, std::string message) {
	for(int i = 0; i < maxClients; i++){
		if(pSocketIsFree[i] == false && pClientSocket[i] != NULL) {
			SDLNet_TCP_Send(pClientSocket[i], (void*)(user.c_str()), 16);
			SDLNet_TCP_Send(pClientSocket[i], (void*)(message.c_str()), 512);
		}
	}
}

int ServerSocket::checkForActivity(std::string& content) {
	int socketNum = SDLNet_CheckSockets(socketSet, 10);
	if(socketNum > 0) {
		for(int clientNumber = 0; clientNumber < maxClients; clientNumber++) {
			int clientSocketActivity = SDLNet_SocketReady(pClientSocket[clientNumber]);
			if(clientSocketActivity != 0) {
				int receivedByteCount = SDLNet_TCP_Recv(pClientSocket[clientNumber], pBuffer, 512);

				if(receivedByteCount <= 0) {
					SDLNet_TCP_DelSocket(socketSet, pClientSocket[clientNumber]);
					SDLNet_TCP_Close(pClientSocket[clientNumber]);
					pClientSocket[clientNumber] = nullptr;
					clientNames[clientNumber] = "";
					pSocketIsFree[clientNumber] = true;
					clientCount--;
				}else{
					content = pBuffer;
					return clientNumber;
				}
			}
		}
	}
	return -1;
}