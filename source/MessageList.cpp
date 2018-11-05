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

void MessageList::addMessage(SDL2_2D_Context& ctx, std::string time, std::string sender, std::string content) {
	int maxXSize = floor(w / ((ctx.getFontX()+1) * scale));
	std::string message = content;
	if(sender != "") {message = time + "|" + sender + ": " + message;}
	int charsY = message.length() / maxXSize + 1;
	totYSize += charsY*(ctx.getFontY()+1)*scale;

	std::vector<link> links;

	int start = message.find("<<");
	int offset = 0;
	while(start != std::string::npos) {
		int end = message.find(">>", offset);
		if(end == -1) {break;};
		links.push_back({start+2, end-1, message.substr(start+2, end-start-2)});
		offset = end+3;
		start = message.find("<<", offset);
	}

	std::string newMessage = "";

	if(links.size() > 0) {
		newMessage = message.substr(0, links[0].start-2);
		for(int i = 0; i < links.size(); i++) {
			newMessage = newMessage + links[i].url;
			if(i == links.size()-1) {
				newMessage = newMessage + message.substr(links[i].end+3);		
			}else{
				newMessage = newMessage + message.substr(links[i].end+3, links[i+1].start-links[i].end-5);
			}
		}
	}

	Message newMessage2 = {time, sender, content, 0, false, newMessage, links};
	list.insert(list.begin(), newMessage2);
}

void MessageList::removeMessage(std::string targetContent) {
	int id = -1;
	for(int i = 0; i < list.size(); i++) {if(list[i].content == targetContent) {id = i; break;}}
	if(id != -1) {list.erase(list.begin()+id);}
}

int MessageList::getTotYSize() {return totYSize;}

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
	if(id != -1) {list[id].writing = true; list[id].oldContent = list[id].content;}
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
			scroll += scrollAmount;
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

void MessageList::drawMessages(SDL2_2D_Context& ctx, bool mode, Colour foreground, Colour background) {
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
				if(list[i].step > 35) {
					list[i].step = 0;
				}else{
					list[i].step++;
				}	
			}else{
				message = list[i].oldContent;
			}

			Text temp(message, x, startY, maxXSize, charsY, scale, foreground, background, ctx);
			int k = 0;
			int offset = 0;
			for(auto& j : list[i].links) {
				temp.addSpecialText({j.start-(2*(k+1))-offset, j.end-(2*(k+1))-offset, {80, 80, 255}}, ctx);
				offset += 2;
				k++;
			}
			ctx.drawText(temp);
			temp.destroyTexture();
		}
	}
}