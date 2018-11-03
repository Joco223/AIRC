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
		std::string pswd;
		ServerSocket* ss;
		ClientSocket* cs;
	};

	std::string getTime();

	NetworkStruct Init(SDL2_2D_Context&, Colour, MessageList&, std::string&, bool&);
	void sendMessage(SDL2_2D_Context&, Colour, NetworkStruct&, std::string, std::string, MessageList&);
	void update(SDL2_2D_Context&, Colour, NetworkStruct&, MessageList&, MessageList&, std::string);
	void updateStatus(NetworkStruct&, std::string, std::string);
	void disconnectUser(NetworkStruct&);
}

