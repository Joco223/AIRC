#include "ClientSocket.h"

ClientSocket::ClientSocket(std::string address, int port_, int bufferSize_) {
	serverHostName = address;
	serverPort = port_;
	bufferSize = bufferSize_;

	pBuffer = new char[bufferSize];
	socketSet = SDLNet_AllocSocketSet(2);
}

bool ClientSocket::connectToServer(SDL2_2D_Context& ctx, std::string pswd, std::string name, MessageList& userList) {
	SDLNet_ResolveHost(&serverIP, serverHostName.c_str(), serverPort);

	clientSocket = SDLNet_TCP_Open(&serverIP);
	SDLNet_TCP_AddSocket(socketSet, clientSocket);
	int activeSockets = SDLNet_CheckSockets(socketSet, 5000);
	int gotServerResponse = SDLNet_SocketReady(clientSocket);
	int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, pBuffer, 3);
	bool accepted = false;

	if(serverResponseByteCount >= 0) {
		if(pBuffer[2] == 'Y') {
			SDLNet_TCP_Send(clientSocket, (void *)(pswd.c_str()), 6);
			SDLNet_TCP_Recv(clientSocket, pBuffer, 1);
			if(pBuffer[0] == '2') {
				accepted = true;
			}
		}else{
			accepted = true;
		}
	}

	if(accepted) {
		SDLNet_TCP_Send(clientSocket, (void *)(name.c_str()), 16);

		while(true) {
			SDLNet_TCP_Recv(clientSocket, pBuffer, 16);
			std::string message = pBuffer;
			if(message != "END") {
				userList.addMessage(ctx, "", "", message);
			}else{
				break;
			}
		}
	}
	return accepted;
}

void ClientSocket::disconnectFromServer() {
	SDLNet_TCP_DelSocket(socketSet, clientSocket);
	SDLNet_TCP_Close(clientSocket);
}

void ClientSocket::sendWritingStatus(std::string input, std::string user) {
	std::string exec;
	if(input != "") {
		exec = "2";
	}else{
		exec = "3";
	}
	SDLNet_TCP_Send(clientSocket, (void*)(exec.c_str()), 1);
	SDLNet_TCP_Send(clientSocket, (void*)(user.c_str()), 16);
}

void ClientSocket::sendMessage(std::string message) {
	std::string exec = "1";
	SDLNet_TCP_Send(clientSocket, (void*)(exec.c_str()), 1);
	std::string messageLength = std::to_string(message.length());
	SDLNet_TCP_Send(clientSocket, (void*)(messageLength.c_str()), 4);
	SDLNet_TCP_Send(clientSocket, (void*)(message.c_str()), message.length());
}

std::string ClientSocket::checkForIncomingMessages(std::string& user, std::string& content, MessageList& userList, bool& received) {
	std::string receivedMessage = "";
	int activeSockets = SDLNet_CheckSockets(socketSet, 10);

	if(activeSockets != 0) {
		int gotMessage = SDLNet_SocketReady(clientSocket);
		if(gotMessage != 0) {
			SDLNet_TCP_Recv(clientSocket, pBuffer, 1);
			std::string exec = pBuffer;
			if(exec[0] == '1') {
				SDLNet_TCP_Recv(clientSocket, pBuffer, 16);
				user = pBuffer;
				SDLNet_TCP_Recv(clientSocket, pBuffer, 4);
				std::string message = pBuffer;
				int finalSize = std::stoi(message);
				SDLNet_TCP_Recv(clientSocket, pBuffer, finalSize);
				content.resize(finalSize);
				for(int i = 0; i < finalSize; i++) {content[i] = pBuffer[i];}
				received = true;
			}else if(exec[0] == '2') {
				SDLNet_TCP_Recv(clientSocket, pBuffer, 16);
				user.resize(16);
				for(int i = 0; i < 16; i++) {user[i] = pBuffer[i];}
				userList.startWriting(user);
				received = false;
			}else{
				SDLNet_TCP_Recv(clientSocket, pBuffer, 16);
				user.resize(16);
				for(int i = 0; i < 16; i++) {user[i] = pBuffer[i];}
				userList.stopWriting(user);
				received = false;
			}	
		}
	}
	return receivedMessage;
}