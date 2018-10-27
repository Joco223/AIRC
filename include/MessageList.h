#pragma once
#include <string>
#include <vector>
#include <math.h>

#include "SDL2_2D.h"

struct Message {
	std::string time;
	std::string sender;
	std::string content;
};

class MessageList {
private:
	std::vector<Message> list;
	int x, y, w, h, scale;
public:
	MessageList(int, int, int, int, int);
	void addMessage(std::string, std::string, std::string);
	void drawMessages(SDL2_2D_Context, Colour);
};