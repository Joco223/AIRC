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

void ServerSocket::checkForConnections(SDL2_2D_Context& ctx, Colour background, std::string pswd, std::string userName, MessageList& userList) {
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
			std::string connected = "OK";
			if(pswd != "") {connected.append("Y");}
			SDLNet_TCP_Send(pClientSocket[freeSpot], (void*)connected.c_str(), 3);

			bool accepted = true;

			if(pswd != "") {
				SDLNet_TCP_Recv(pClientSocket[freeSpot], pBuffer, 6);

				std::string userPswd = pBuffer;
				userPswd = userPswd.substr(0, 6);

				std::string correct = "2";
				std::string incorrect = "1";
				

				if(userPswd == pswd) {
					SDLNet_TCP_Send(pClientSocket[freeSpot], correct.c_str(), 1);		
				}else{
					SDLNet_TCP_Send(pClientSocket[freeSpot], incorrect.c_str(), 1);
					accepted = false;
				}
			}

			if(accepted) {
				SDLNet_TCP_Recv(pClientSocket[freeSpot], pBuffer, 16);
				clientNames[freeSpot] = pBuffer;
				userList.addMessage(ctx, background, "", "", clientNames[freeSpot]);
				std::string empty = "";

				for(int i = 0; i < maxClients; i++) {
					if((i != freeSpot) && (pSocketIsFree[i] == false)) {
						SDLNet_TCP_Send(pClientSocket[i], (void*)(clientNames[freeSpot].c_str()), 16);
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
				SDLNet_TCP_DelSocket(socketSet, pClientSocket[freeSpot]);
				SDLNet_TCP_Close(pClientSocket[freeSpot]);
				pClientSocket[freeSpot] = nullptr;
				clientNames[freeSpot] = "";
				pSocketIsFree[freeSpot] = true;
				clientCount--;
			}	
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
			std::string exec = "1";
			SDLNet_TCP_Send(pClientSocket[i], (void*)(exec.c_str()), 1);
			SDLNet_TCP_Send(pClientSocket[i], (void*)(clientNames[i].c_str()), 16);
			std::string messageLength = std::to_string(message.length());
			SDLNet_TCP_Send(pClientSocket[i], (void*)(messageLength.c_str()), 4);
			SDLNet_TCP_Send(pClientSocket[i], (void*)(message.c_str()), message.length());
		}
	}
}

void ServerSocket::sendMessages(std::string user, std::string message) {
	for(int i = 0; i < maxClients; i++){
		if(pSocketIsFree[i] == false && pClientSocket[i] != NULL) {
			std::string exec = "1";
			SDLNet_TCP_Send(pClientSocket[i], (void*)(exec.c_str()), 1);
			SDLNet_TCP_Send(pClientSocket[i], (void*)(user.c_str()), 16);
			std::string messageLength = std::to_string(message.length());
			SDLNet_TCP_Send(pClientSocket[i], (void*)(messageLength.c_str()), 4);
			SDLNet_TCP_Send(pClientSocket[i], (void*)(message.c_str()), message.length());
		}
	}
}

void ServerSocket::sendWritingStatus(std::string input, std::string user) {
	for(int i = 0; i < maxClients; i++){
		if(pSocketIsFree[i] == false && pClientSocket[i] != NULL) {
			std::string exec;
			if(input != "") {
				exec = "2";
			}else{
				exec = "3";
			}
			SDLNet_TCP_Send(pClientSocket[i], (void*)(exec.c_str()), 1);
			SDLNet_TCP_Send(pClientSocket[i], (void*)(user.c_str()), 16);
		}
	}
}

int ServerSocket::checkForActivity(std::string& content, MessageList& userList) {
	int socketNum = SDLNet_CheckSockets(socketSet, 10);
	if(socketNum > 0) {
		for(int clientNumber = 0; clientNumber < maxClients; clientNumber++) {
			int clientSocketActivity = SDLNet_SocketReady(pClientSocket[clientNumber]);
			if(clientSocketActivity != 0) {
				int receivedByteCount = SDLNet_TCP_Recv(pClientSocket[clientNumber], pBuffer, 1);
				std::string exec = pBuffer;
				
				if(receivedByteCount <= 0) {
					SDLNet_TCP_DelSocket(socketSet, pClientSocket[clientNumber]);
					SDLNet_TCP_Close(pClientSocket[clientNumber]);
					pClientSocket[clientNumber] = nullptr;
					userList.removeMessage(clientNames[clientNumber]);
					clientNames[clientNumber] = "";
					pSocketIsFree[clientNumber] = true;
					clientCount--;
				}else{
					if(exec[0] == '1') {
						SDLNet_TCP_Recv(pClientSocket[clientNumber], pBuffer, 4);
						std::string message = pBuffer;
						int finalSize = std::stoi(message);
						SDLNet_TCP_Recv(pClientSocket[clientNumber], pBuffer, finalSize);
						content.resize(finalSize);
						for(int i = 0; i < finalSize; i++) {content[i] = pBuffer[i];}
						return clientNumber;
					}else if(exec[0] == '2') {
						SDLNet_TCP_Recv(pClientSocket[clientNumber], pBuffer, 16);
						std::string message;
						message.resize(16);
						for(int i = 0; i < 16; i++) {message[i] = pBuffer[i];}
						userList.startWriting(message);
						return -1;
					}else if(exec[0] == '3') {
						SDLNet_TCP_Recv(pClientSocket[clientNumber], pBuffer, 16);
						std::string message;
						message.resize(16);
						for(int i = 0; i < 16; i++) {message[i] = pBuffer[i];}
						userList.stopWriting(message);
						return -1;
					}		
				}
			}
		}
	}
	return -1;
}