#include "Networking.h"

namespace Networking {

	std::string getTime() {
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		std::string hours = std::to_string(now->tm_hour);
		if(now->tm_hour < 10) { hours = "0" + hours; }
		std::string minutes = std::to_string(now->tm_min);
		if(now->tm_min < 10) { minutes = "0" + minutes; }
		std::string time = hours + ":" + minutes;
		return time;
	}

	void disconnectUser(NetworkStruct& NS) {
		if(!NS.host) {
			NS.cs->disconnectFromServer();
		}
	}

	NetworkStruct Init(SDL2_2D_Context& ctx, MessageList& userList, std::string& user, bool& connected) {
		std::ifstream inputF("config.cfg");

		int index = 0;
		std::string hostMode;
		std::string ipAddress;
		std::string password = "";
		bool host = false;
		for(std::string line; std::getline(inputF, line);) {
			switch(index) {
				case 1:
					user = line;
					break;
				case 3:
					hostMode = line;
					if(line == "host") { host = true; }
					break;
	            case 5:
					ipAddress = line;
					break;
				case 7:
					password = line;
					break;
			}
			index++;
		}

		ServerSocket* ss;
		ClientSocket* cs;

		NetworkStruct NS = {host, password, ss, cs};
		
		if(hostMode == "host") {
			NS.ss = new ServerSocket(12000, 512, 50);
			ctx.setWindowTitle("AIRC - " + NS.ss->dotQuadString);
			connected = true;
		}else{
			NS.cs = new ClientSocket(ipAddress, 12000, 512);
			connected = NS.cs->connectToServer(ctx, password, user, userList);
			ctx.setWindowTitle("AIRC - " + ipAddress);
		}

		return NS;
	}

	void sendMessage(SDL2_2D_Context& ctx, NetworkStruct& NS, std::string input, std::string user, MessageList& messageList, bool& needsUpdating) {
		if(NS.host) {
			NS.ss->sendMessages(user, input);
		}else{
			NS.cs->sendMessage(input);
		}

		messageList.addMessage(ctx, getTime(), user, input);
		needsUpdating = true;
	}

	void updateStatus(NetworkStruct& NS, std::string input, std::string user) {
		if(NS.host) {
			NS.ss->sendWritingStatus(input, user);
		}else{
			NS.cs->sendWritingStatus(input, user);
		}
	}

	void update(SDL2_2D_Context& ctx, NetworkStruct& NS, MessageList& messageList, MessageList& userList, std::string user, bool& needsUpdating) {
		if(NS.host) {
			NS.ss->checkForConnections(ctx, NS.pswd, user, userList);
			std::string userName;
			std::string messageContnet;

			int activeClient = NS.ss->checkForActivity(messageContnet, userList);

			while(activeClient != -1) {
				if(activeClient != -1) {
					NS.ss->dealWithActivity(activeClient, userName, messageContnet);
					if(messageContnet != "") {
						messageList.addMessage(ctx, getTime(), userName, messageContnet);
						needsUpdating = true;
					}
				}
				activeClient = NS.ss->checkForActivity(messageContnet, userList);
			}
		}else{
			std::string userName;
			std::string messageContnet;
			bool received = false;
			NS.cs->checkForIncomingMessages(userName, messageContnet, userList, received);
			if(messageContnet != "") {
				messageList.addMessage(ctx, getTime(), userName, messageContnet);
				needsUpdating = true;
			}else if(received){
				userList.addMessage(ctx, "", "", userName);
				needsUpdating = true;
			}
		}
	}
}