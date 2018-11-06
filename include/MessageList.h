#pragma once
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <cstdlib>

#include "SDL2_2D.h"

struct link {
	int start;
	int end;
	std::string url;
	int newStart;
	int newEnd;
};

struct Message {
	std::string time;
	std::string sender;
	std::string content;
	int step;
	bool writing;
	std::string oldContent;
	std::vector<link> links;
	int posY, charX, charY;
	bool linksAdded;
};

struct clickableLink {
	int x; 
	int y; 
	int xSize;
	int ySize;
	int msgIndex;
	int index;
};

class MessageList {
private:
	std::string writingSign = "/////////---------\\\\\\\\\\\\\\\\\\|||||||||";
	std::vector<Message> list;
	std::vector<clickableLink> clickLinks;
	int x, y, w, h, scale, seperation, scroll, totYSize;
public:
	MessageList(int, int, int, int, int, int);
	void startWriting(std::string);
	void stopWriting(std::string);
	void addMessage(SDL2_2D_Context&, std::string, std::string, std::string);
	void removeMessage(std::string);
	void drawMessages(SDL2_2D_Context&, bool, Colour, Colour);
	void scrollMessages(int, int, int);
	void checkClicks(int, int);
	int getTotYSize();
};