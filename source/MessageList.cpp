#include "MessageList.h"

MessageList::MessageList(int x_, int y_, int w_, int h_, int scale_, int seperation_) 
			:
			scale(scale_),
			x(x_),
			y(y_),
			w(w_),
			h(h_),
			seperation(seperation_)
			{};

void MessageList::addMessage(std::string time, std::string sender, std::string content) {
	Message newMessage = {time, sender, content, 0, false};
	list.insert(list.begin(), newMessage);
}

void MessageList::removeMessage(std::string targetContent) {
	int id = -1;
	for(int i = 0; i < list.size(); i++) {if(list[i].content == targetContent) {id = i; break;}}
	if(id != -1) {list.erase(list.begin()+id);}
}

void MessageList::startWriting(std::string user) {
	int id = -1;
	for(int i = 0; i < list.size(); i++) {
		bool correct = true;

		for(int j = 0; j < list[i].content.length(); j++){
			if(list[i].content[j] != user[j]) {
				correct = false;
				break;
			}
		}

		if(correct) {
			id = i; 
			break;
		}
	}
	if(id != -1) {list[id].writing = true;}
}

void MessageList::stopWriting(std::string user) {
	int id = -1;
	for(int i = 0; i < list.size(); i++) {
		bool correct = true;

		for(int j = 0; j < list[i].content.length(); j++){
			if(list[i].content[j] != user[j]) {
				correct = false;
				break;
			}
		}

		if(correct) {
			id = i; 
			break;
		}
	}
	if(id != -1) {list[id].writing = false; list[id].step = 0;}
}

void MessageList::drawMessages(SDL2_2D_Context ctx, Colour windowColour, bool mode) {
	int yOffset = 0;
	int maxXSize = floor(w / ((ctx.getFontX()+1) * scale));
	for(int i = 0; i < list.size(); i++) {
		std::string message = list[i].content;
		if(list[i].sender != "") {message = list[i].time + "|" + list[i].sender + ": " + message;}
		int charsY = message.length() / maxXSize;
		if(message.length() % maxXSize != 0) {charsY++;}
		int sizeY = charsY*(ctx.getFontY()+seperation)*scale;
		int startY;
		if(mode) {
			startY = y+h-sizeY-yOffset;
		}else{
			startY = y+yOffset+1;
		}		
		yOffset += sizeY;
		if(startY <= (y+h)) {
			if(list[i].writing) {
				int difference = 0;
				if(message.length() < maxXSize) {difference = maxXSize - message.length();}
				for(int j = 0; j < difference; j++) {message.append(".");}
				message[message.length() - 1] = writingSign[list[i].step];
				if(list[i].step > 23) {
					list[i].step = 0;
				}else{
					list[i].step++;
				}	
			}
			ctx.drawText(message, x, startY, maxXSize, charsY, scale, {255, 255, 255}, windowColour);
		}else{
			break;
		}
	}
}