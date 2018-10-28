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
				for(int i = 0; i < maxClients; i++) {pClientSocket[i] = nullptr; pSocketIsFree[i] = false;}
				SDLNet_ResolveHost(&serverIP, NULL, port);

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

void ServerSocket::checkForConnections() {
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
		}else{
			TCPsocket tempSock = SDLNet_TCP_Accept(serverSocket);
			strcpy(pBuffer, "FULL");
			int msgLength = 5;
			SDLNet_TCP_Send(pClientSocket[freeSpot], (void*)pBuffer, msgLength);
			SDLNet_TCP_Close(tempSock);
		}
	}
}

void ServerSocket::dealWithActivity(unsigned int clientNumber, std::string& message) {
	std::string bufferContents = pBuffer;
	for(int i = 0; i < maxClients; i++) {
		int msgLength = bufferContents.length();
		if((msgLength > 1) && (i != clientNumber) && (pSocketIsFree[i] == false)) {
			std::string final = clientNames[i] + bufferContents;
			std::cout << bufferContents << '\n';
			message = bufferContents;
			const char* finalB = final.c_str();
			//SDLNet_TCP_Send(pClientSocket[i], (void*)finalB, 512);
		}
	}
}

void ServerSocket::sendMessages(std::string message) {
	for(int i = 0; i < maxClients; i++){
		std::cout << pClientSocket[i] << '\n';
		if(pSocketIsFree[i] == false && pClientSocket[i] != NULL) {
			SDLNet_TCP_Send(pClientSocket[i], (void*)(message.c_str()), 512);
		}
	}
}

int ServerSocket::checkForActivity() {
	int socketNum = SDLNet_CheckSockets(socketSet, 10);
	if(socketNum > 0) {
		for(int clientNumber = 0; clientNumber < maxClients; clientNumber++) {
			int clientSocketActivity = SDLNet_SocketReady(pClientSocket[clientNumber]);
			std::cout << clientSocketActivity << '\n';
			if(clientSocketActivity != 0) {
				int receivedByteCount = SDLNet_TCP_Recv(pClientSocket[clientNumber], pBuffer, bufferSize);

				std::cout << receivedByteCount << '\n';

				if(receivedByteCount <= 0) {
					SDLNet_TCP_DelSocket(socketSet, pClientSocket[clientNumber]);
					SDLNet_TCP_Close(pClientSocket[clientNumber]);
					pClientSocket[clientNumber] = nullptr;
					clientNames[clientNumber] = "";
					pSocketIsFree[clientNumber] = true;
					clientCount--;
				}else{
					return clientNumber;
				}
			}
		}
	}
	return -1;
}