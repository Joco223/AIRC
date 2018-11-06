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
	int additionalSize = 0;
	if(sender != "") {message = time + "|" + sender + ": " + message; additionalSize = time.length() + 3 + sender.length();}
	
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
	}else{
		newMessage = message;
	}

	int charsY = newMessage.length() / maxXSize + 1;
	totYSize += charsY*(ctx.getFontY()+1)*scale;

	int messageCharX;
	if(charsY == 1) {
		messageCharX = newMessage.length();
	}else{
		messageCharX = maxXSize;
	};

	Message newMessage2 = {time, sender, message.substr(additionalSize), 0, false, newMessage, links, 0, messageCharX, charsY, false};
	list.insert(list.begin(), newMessage2);

	for(int i = 0; i < clickLinks.size(); i++) {
		clickLinks[i].y -= charsY*(ctx.getFontY()+1)*scale;
		clickLinks[i].msgIndex++;
	}
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
		list[i].posY = startY;
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
				j.newStart = j.start-(2*(k+1))-offset;
				j.newEnd = j.end-(2*(k+1))-offset;
				offset += 2;
				k++;
			}
			ctx.drawText(temp);
			temp.destroyTexture();

			if(list[i].links.size() > 0 && !list[i].linksAdded) {		
				for(int j = 0; j < list[i].links.size(); j++){
					std::cout << list[i].links[j].newEnd << " " << list[i].charX << '\n';
					int linkYOffset = list[i].links[j].newStart / list[i].charX;
					int newStartPos = list[i].links[j].newStart;
					int newEndPos = list[i].links[j].newEnd;
					if(linkYOffset > 0) {
						newStartPos -= list[i].charX;
						newEndPos -= list[i].charX;
					}
					if(list[i].links[j].newEnd <= list[i].charX) {
						clickableLink newLink1 = {newStartPos*(ctx.getFontX()+1)*scale, list[i].posY+(linkYOffset*(ctx.getFontY()+1)*scale), (newEndPos-newStartPos)*(ctx.getFontX()+1)*scale, (ctx.getFontY()+1)*scale, i, j};
						clickLinks.insert(clickLinks.begin(), newLink1);
					}else{
						clickableLink newLink1 = {newStartPos*(ctx.getFontX()+1)*scale, list[i].posY+(linkYOffset*(ctx.getFontY()+1)*scale), (list[i].charX-newStartPos)*(ctx.getFontX()+1)*scale, (ctx.getFontY()+1)*scale, i, j};
						clickLinks.insert(clickLinks.begin(), newLink1);
						clickableLink newLink2 = {x, list[i].posY+((linkYOffset+1)*(ctx.getFontY()+1)*scale), (newEndPos-list[j].charX)*(ctx.getFontX()+1)*scale, (ctx.getFontY()+1)*scale, i, j};
						clickLinks.insert(clickLinks.begin(), newLink2);
					}	
				}
				list[i].linksAdded = true;
			}
		}
	}
}

void MessageList::checkClicks(int clickX, int clickY) {
	for(int i = 0; i < clickLinks.size(); i++)  {
		//std::cout << clickLinks[i].x << " " << clickLinks[i].y << " " << clickX << " " << clickY << '\n';
		if(clickX >= clickLinks[i].x+scroll && clickX <= clickLinks[i].x+clickLinks[i].xSize+scroll) {
			if(clickY >= clickLinks[i].y+scroll && clickY <= clickLinks[i].y+clickLinks[i].ySize+scroll) {
				std::string URL = "start " + list[clickLinks[i].msgIndex].links[clickLinks[i].index].url;
				std::cout << URL << '\n';
				std::system(URL.c_str());
				break;
			}
		}
		
	}
}