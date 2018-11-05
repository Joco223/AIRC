#pragma once
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include "SDL2_2D.h"

struct link {
	int start;
	int end;
	std::string url;
};

struct Message {
	std::string time;
	std::string sender;
	std::string content;
	int step;
	bool writing;
	std::string oldContent;
	std::vector<link> links;
};

class MessageList {
private:
	std::string writingSign = "/////////---------\\\\\\\\\\\\\\\\\\|||||||||";
	std::vector<Message> list;
	int x, y, w, h, scale, seperation, scroll, totYSize;
public:
	MessageList(int, int, int, int, int, int);
	void startWriting(std::string);
	void stopWriting(std::string);
	void addMessage(SDL2_2D_Context&, std::string, std::string, std::string);
	void removeMessage(std::string);
	void drawMessages(SDL2_2D_Context&, bool, Colour, Colour);
	void scrollMessages(int, int, int);
	int getTotYSize();
};