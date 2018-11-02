#pragma once

#include <string>
#include <ctime>
#include <fstream>

#include "ServerSocket.h"
#include "ClientSocket.h"
#include "MessageList.h"
#include "SDL2_2D.h"

namespace Networking {
	struct NetworkStruct {
		bool host;
		ServerSocket* ss;
		ClientSocket* cs;
	};

	std::string getTime();

	NetworkStruct Init(SDL2_2D_Context&, MessageList&, std::string&);
	void sendMessage(NetworkStruct&, std::string, std::string, MessageList&);
	void update(NetworkStruct&, MessageList&, MessageList&, std::string);
	void updateStatus(NetworkStruct&, std::string, std::string);
}

