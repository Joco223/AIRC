#include "MessageList.h"

MessageList::MessageList(int x_, int y_, int w_, int h_, int scale_, int seperation_) 
			:
			scale(scale_),
			x(x_),
			y(y_),
			w(w_),
			h(h_),
			scroll(0),
			totYSize(0),
			seperation(seperation_)
			{};

void MessageList::addMessage(SDL2_2D_Context& ctx, Colour background, std::string time, std::string sender, std::string content) {
	/*int maxXSize = floor(w / ((ctx.getFontX()+1) * scale));
	std::string message = content;
	if(sender != "") {message = time + "|" + sender + ": " + message;}
	int charsY = message.length() / maxXSize;
	if(message.length() % maxXSize != 0) {charsY++;}
	Text temp(message, 0, 0, maxXSize, charsY, scale, {255, 255, 255}, background, ctx);*/

	int maxXSize = floor(w / ((ctx.getFontX()+1) * scale));
	std::string message = content;
	if(sender != "") {message = time + "|" + sender + ": " + message;}
	int charsY = message.length() / maxXSize + 1;
	totYSize += charsY*(ctx.getFontY()+1)*scale;
	Message newMessage = {time, sender, message, 0, false, message};
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
	if(id != -1) {list[id].writing = true; 
		list[id].oldContent = list[id].content;
	}
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

void MessageList::scrollMessages(int mouseX, int mouseY, int scrollAmount) {
	if(mouseX > x && mouseX < x + w) {
		if(mouseY > y && mouseY < y + h) {
			scroll += scrollAmount*20;
			if(scroll < 0) {scroll = 0;}
			if(scroll > (y+h - totYSize) * -1) {
				if((y+h - totYSize) * -1 > 0) {
					scroll = (y+h - totYSize) * -1;
				}else{
					scroll = 0;
				}
			}
		}
	}
}

void MessageList::drawMessages(SDL2_2D_Context& ctx, bool mode) {
	int maxXSize = floor(w / ((ctx.getFontX()+1) * scale));
	int yOffset = 0;
	for(int i = 0; i < list.size(); i++) {
		int startY = 0;
		std::string message = list[i].content;
		if(list[i].sender != "") {message = list[i].time + "|" + list[i].sender + ": " + message;}
		int charsY = message.length() / maxXSize;
		if(message.length() % maxXSize != 0) {charsY++;}
		int sizeY = charsY*(ctx.getFontY()+1)*scale;
		if(mode) {startY = y+h-sizeY-yOffset;}else{startY = y+yOffset+1;}
		if(startY <= y+h) {startY += scroll;}
		yOffset += sizeY;
		if(startY <= y+h) {
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
			}else{
				message = list[i].oldContent;
			}

			Text temp(message, x, startY, maxXSize, charsY, scale, {255, 255, 255}, {50, 50, 50}, ctx);
			ctx.drawText(temp);
			temp.destroyTexture();
		}
	}
}