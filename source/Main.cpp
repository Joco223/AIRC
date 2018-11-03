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

    SDL2_2D_Context ctx;

    SDLNet_Init();

    ctx.Init("AIRC", w, h, windowColour);

    SDL_Event e;

    Rect UserDivider(w-175, 0, 3, h, {255, 255, 255});
    Rect InputDivider(0, h-20, w-175, 2, {255, 255, 255});

    ctx.loadFont("Font.bmp", 5, 7);

    MessageList messageList(0, 0, w-175, h-19, scale, 3);
    MessageList userList(w-170, 0, 170, h, scale, 2);

    std::string input;
    std::string user;

    bool connected = false;
    Networking::NetworkStruct NS = Networking::Init(ctx, windowColour, userList, user, connected);

    if(!connected) {return 0;}

    userList.addMessage(ctx, windowColour, "", "", user);
    bool alreadyUpdated = true;
    
    SDL_StartTextInput();
        
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
                    }else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                        SDL_SetClipboardText(input.c_str());
                    }else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                        input += SDL_GetClipboardText();
                    }else if(e.key.keysym.sym == SDLK_KP_ENTER || e.key.keysym.sym == SDLK_RETURN) {
                        if(input.length() > 0) {
                            Networking::sendMessage(ctx, windowColour, NS, input, user, messageList);
                            input = "";
                        }   
                    }
                    break;
                case SDL_TEXTINPUT:
                    if(!((e.text.text[0] == 'c' || e.text.text[0] == 'C' ) && ( e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
                        input += e.text.text;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    messageList.scrollMessages(x, y, e.wheel.y*20);
                    break;
            }
        }
        if(input != "") {
            if(alreadyUpdated) {
                userList.startWriting(user);
                Networking::updateStatus(NS, input, user);
                alreadyUpdated = false; 
            }    
        }else{
            if(!alreadyUpdated) {
                userList.stopWriting(user);
                Networking::updateStatus(NS, input, user);
                alreadyUpdated = true;
            }   
        }

        Networking::update(ctx, windowColour, NS, messageList, userList, user);

        ctx.clear();
        messageList.drawMessages(ctx, true);
        ctx.drawRect(w-172, 0, 172, h, windowColour);
        userList.drawMessages(ctx, false);
        ctx.drawRect(UserDivider, nullptr, 0);
        ctx.drawRect(InputDivider, nullptr, 0);
        ctx.drawRect(0, h-18, w-175, 18, windowColour);
        std::string inputText = input + "_";
        int maxXSize = (w-175) / ((ctx.getFontX()+1) * 2);
        if(inputText.length() > maxXSize) {
            int difference = inputText.length() - maxXSize;
            inputText.erase(0, difference);
            ctx.drawText(inputText, 0, h-16, maxXSize, 1, 2, {255, 255, 255}, windowColour);
        }else{
            ctx.drawText(inputText, 0, h-16, maxXSize, 1, 2, {255, 255, 255}, windowColour);
        }
        ctx.draw();
    }

    SDL_StopTextInput();
    ctx.deInit();

	return 0;
}