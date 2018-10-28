#include "ClientSocket.h"

ClientSocket::ClientSocket(std::string address, int port_, int bufferSize_) {
	serverHostName = address;
	serverPort = port_;
	bufferSize = bufferSize_;

	pBuffer = new char[bufferSize];
	socketSet = SDLNet_AllocSocketSet(2);
}

void ClientSocket::connectToServer(std::string name, MessageList& userList) {
	SDLNet_ResolveHost(&serverIP, serverHostName.c_str(), serverPort);

	clientSocket = SDLNet_TCP_Open(&serverIP);
	SDLNet_TCP_AddSocket(socketSet, clientSocket);
	int activeSockets = SDLNet_CheckSockets(socketSet, 5000);
	int gotServerResponse = SDLNet_SocketReady(clientSocket);
	int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, pBuffer, 3);
	SDLNet_TCP_Send(clientSocket, (void *)(name.c_str()), 16);

	while(true) {
		SDLNet_TCP_Recv(clientSocket, pBuffer, 16);
		std::string message = pBuffer;
		if(message != "END") {
			userList.addMessage("", "", message);
		}else{
			break;
		}
	}
}

void ClientSocket::sendMessage(std::string message) {
	SDLNet_TCP_Send(clientSocket, (void *)(message.c_str()), 512);
}

std::string ClientSocket::checkForIncomingMessages(std::string& user, std::string& content) {
	std::string recievedMessage = "";
	int activeSockets = SDLNet_CheckSockets(socketSet, 10);

	if(activeSockets != 0) {
		int gotMessage = SDLNet_SocketReady(clientSocket);
		if(gotMessage != 0) {
			SDLNet_TCP_Recv(clientSocket, pBuffer, 16);
			user = pBuffer;
			SDLNet_TCP_Recv(clientSocket, pBuffer, 512);
			content = pBuffer;
		}
	}

	return recievedMessage;
}