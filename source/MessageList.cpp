#include "MessageList.h"

MessageList::MessageList(int x_, int y_, int w_, int h_, int scale_) 
			:
			scale(scale_),
			x(x_),
			y(y_),
			w(w_),
			h(h_) 
			{};

void MessageList::addMessage(std::string time, std::string sender, std::string content) {
	Message newMessage = {time, sender, content};
	list.insert(list.begin(), newMessage);
}

void MessageList::drawMessages(SDL2_2D_Context ctx, Colour windowColour) {
	int yOffset = 0;
	int maxXSize = floor(w / ((ctx.getFontX()+1) * scale));
	for(int i = 0; i < list.size(); i++) {
		std::string message = list[i].time + "|" + list[i].sender + ": " + list[i].content;
		int charsY = message.length() / maxXSize + 1;
		int sizeY = charsY*(ctx.getFontY()+1)*scale;
		int startY = y+h-sizeY-yOffset;
		yOffset += sizeY;
		if(startY <= (y+h)) {	
			ctx.drawText(message, x, startY, maxXSize, charsY, scale, {255, 255, 255}, windowColour);
		}else{
			break;
		}
	}
}