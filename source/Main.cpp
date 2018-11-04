#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_Main.h>
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

#include "SDL2_2D.h"
#include "MessageList.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Networking.h"

constexpr int w = 960, h = 540;
constexpr int scale = 2;
bool quit = false;

int main(int argc, char* argv[]) {

	Colour windowColour = {50, 50, 50};
	Colour fontColour = {255, 255, 255};
	Colour GUIColour = {255, 255, 255};

	SDL2_2D_Context ctx;

	SDLNet_Init();

	ctx.Init("AIRC", w, h, windowColour);

	SDL_Event e;

	Rect UserDivider(w-175, 0, 3, h, GUIColour);
	Rect InputDivider(0, h-20, w-175, 2, GUIColour);

	ctx.loadFont("Font.bmp", 5, 7);

	MessageList messageList(0, 0, w-175, h-19, scale, 3);
	MessageList userList(w-170, 0, 170, h, scale, 2);

	std::string input;
	std::string user;

	bool connected = false;
	Networking::NetworkStruct NS = Networking::Init(ctx, userList, user, connected);

	if(!connected) {return 0;}

	userList.addMessage(ctx, "", "", user);
	bool alreadyUpdated = true;
	
	SDL_StartTextInput();

	bool needsUpdating = true;
	bool inputNeedsUpdating = true;
	bool firstTime = true;

	int mX, mY, scroll;
	bool scrolled = true;
		
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					Networking::disconnectUser(NS);
					break;
				case SDL_KEYDOWN:
					if(e.key.keysym.sym == SDLK_BACKSPACE && input.length() > 0) {
						input.pop_back();
						needsUpdating = true;
					}else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
						SDL_SetClipboardText(input.c_str());
					}else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
						input += SDL_GetClipboardText();
						inputNeedsUpdating = true;
					}else if(e.key.keysym.sym == SDLK_KP_ENTER || e.key.keysym.sym == SDLK_RETURN) {
						if(input.length() > 0) {
							Networking::sendMessage(ctx, NS, input, user, messageList, needsUpdating);
							input = "";
						}   
					}
					break;
				case SDL_TEXTINPUT:
					if(!((e.text.text[0] == 'c' || e.text.text[0] == 'C' ) && ( e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
						input += e.text.text;
						inputNeedsUpdating = true;
					}
					break;
				case SDL_MOUSEWHEEL:
					int x, y;
					SDL_GetMouseState(&x, &y);
					mX = x;
					mY = y;
					scroll = e.wheel.y*12;
					messageList.scrollMessages(x, y, e.wheel.y*6);
					needsUpdating = true;
					break;
			}
		}
		if(input != "") {
			inputNeedsUpdating = true;
			if(alreadyUpdated) {
				userList.startWriting(user);
				Networking::updateStatus(NS, input, user);
				alreadyUpdated = false; 
				needsUpdating = true;
			}    
		}else{
			if(!alreadyUpdated) {
				userList.stopWriting(user);
				Networking::updateStatus(NS, input, user);
				alreadyUpdated = true;
				needsUpdating = true;
			}   
		}

		if(scroll != 0) {
			messageList.scrollMessages(mX, mY, scroll/2);
			scroll /= 2;
			if(!scrolled) {
				messageList.scrollMessages(mX, mY, scroll);
				scrolled = 0;
				scrolled = true;
			}
			needsUpdating = true;
			scrolled = false;
		}

		Networking::update(ctx, NS, messageList, userList, user, needsUpdating);

		if(needsUpdating || firstTime) {
			ctx.clear();
			messageList.drawMessages(ctx, true, fontColour, windowColour);
			ctx.drawRect(w-172, 0, 172, h, windowColour);
			userList.drawMessages(ctx, false, fontColour, windowColour);
			ctx.drawRect(UserDivider, nullptr, 0);
			ctx.drawRect(InputDivider, nullptr, 0);
			ctx.drawRect(0, h-18, w-175, 18, windowColour);
			std::string inputText = input + "_";
			int maxXSize = (w-175) / ((ctx.getFontX()+1) * 2);
			if(inputText.length() > maxXSize) {
				int difference = inputText.length() - maxXSize;
				inputText.erase(0, difference);
				ctx.drawText(inputText, 0, h-16, maxXSize, 1, 2, fontColour, windowColour);
			}else{
				ctx.drawText(inputText, 0, h-16, maxXSize, 1, 2, fontColour, windowColour);
			}
			ctx.draw();
			if(!needsUpdating) {firstTime = false;}
			needsUpdating = false;
		}


		if(inputNeedsUpdating) {
			ctx.drawRect(w-172, 0, 172, userList.getTotYSize(), windowColour);
			userList.drawMessages(ctx, false, fontColour, windowColour);
			std::string inputText = input + "_";
			int maxXSize = (w-175) / ((ctx.getFontX()+1) * 2);
			if(inputText.length() > maxXSize) {
				int difference = inputText.length() - maxXSize;
				inputText.erase(0, difference);
				ctx.drawText(inputText, 0, h-16, maxXSize, 1, 2, fontColour, windowColour);
			}else{
				ctx.drawText(inputText, 0, h-16, maxXSize, 1, 2, fontColour, windowColour);
			}
			ctx.draw();
		}
	}

	SDL_StopTextInput();
	ctx.deInit();

	return 0;
}