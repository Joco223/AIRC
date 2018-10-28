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

constexpr int w = 640, h = 360;
constexpr int scale = 2;
bool quit = false;

int main(int argc, char* argv[]) {

	Colour windowColour = {50, 50, 50};

    SDL2_2D_Context ctx;

    SDLNet_Init();

    ctx.Init("AIRC", w, h, windowColour);

    SDL_Event e;

    Rect UserDivider(507, 0, 3, h, {255, 255, 255});
    Rect InputDivider(0, 341, 507, 3, {255, 255, 255});

    ctx.loadFont("Font.bmp", 5, 7);

    MessageList messageList(0, 0, 507, 341, scale);
    MessageList userList(511, 0, 130, 360, scale);

    std::string input;

    std::ifstream inputF("config.cfg");

    int index = 0;
    std::string user;
    std::string hostMode;
    std::string ipAddress;
    for(std::string line; std::getline(inputF, line);) {
        bool host = false;
        switch(index) {
            case 0:
                user = line;
                break;
            case 1:
                hostMode = line;
                if(line == "host") { host = true; }
                break;
            case 2:
                ipAddress = line;
                break;
        }
        if(host) {break;}
        index++;
    }

    userList.addMessage("", "", user);

    int activeClient = -1;
    ServerSocket* ss;
    ClientSocket* cs;

    if(hostMode == "host") {
        ss = new ServerSocket(12000, 512, 50);
        ctx.setWindowTitle("AIRC " + ss->dotQuadString);
    }else{
        cs = new ClientSocket(ipAddress, 12000, 512);
        cs->connectToServer(user, userList);
        ctx.setWindowTitle("AIRC " + ipAddress);
    }
    std::string recievedMessage;
    bool first = true;

    SDL_StartTextInput();
        
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_BACKSPACE && input.length() > 0) {
                        input.pop_back();
                    }else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
                        SDL_SetClipboardText( input.c_str() );
                    }else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
                        input += SDL_GetClipboardText();
                    }else if(e.key.keysym.sym == SDLK_KP_ENTER || e.key.keysym.sym == SDLK_RETURN) {
                        if(input.length() > 0) {

                            if(hostMode == "host") {
                                ss->sendMessages(user, input);
                            }else{
                                cs->sendMessage(input);
                            }
                            
                            std::time_t t = std::time(0);
                            std::tm* now = std::localtime(&t);
                            std::string hours = std::to_string(now->tm_hour);
                            if(now->tm_hour < 10) { hours = "0" + hours; }
                             std::string minutes = std::to_string(now->tm_min);
                            if(now->tm_min < 10) { minutes = "0" + minutes; }
                            std::string time = hours + ":" + minutes;
                            messageList.addMessage(time, user, input);
                            input = "";
                        }   
                    }
                    break;
                case SDL_TEXTINPUT:
                    if(!((e.text.text[0] == 'c' || e.text.text[0] == 'C' ) && ( e.text.text[0] == 'v' || e.text.text[0] == 'V' ) && SDL_GetModState() & KMOD_CTRL)) {
                        input += e.text.text;
                    }
                    break;
            }
            ctx.clear();

            if(hostMode == "host") {
                ss->checkForConnections(user, userList);
                std::string userName;
                std::string messageContnet;

                activeClient = ss->checkForActivity(messageContnet);

                while(activeClient != -1) {
                    if(activeClient != -1) {
                        ss->dealWithActivity(activeClient, userName, messageContnet);
                        if(messageContnet != "") {
                            std::time_t t = std::time(0);
                            std::tm* now = std::localtime(&t);
                            std::string hours = std::to_string(now->tm_hour);
                            if(now->tm_hour < 10) { hours = "0" + hours; }
                            std::string minutes = std::to_string(now->tm_min);
                            if(now->tm_min < 10) { minutes = "0" + minutes; }
                            std::string time = hours + ":" + minutes;
                            messageList.addMessage(time, userName, messageContnet);
                        }
                    }
                    activeClient = ss->checkForActivity(messageContnet);
                }
            }else{
                std::string userName;
                std::string messageContnet;
                cs->checkForIncomingMessages(userName, messageContnet);
                if(messageContnet != "") {
                    std::time_t t = std::time(0);
                    std::tm* now = std::localtime(&t);
                    std::string hours = std::to_string(now->tm_hour);
                    if(now->tm_hour < 10) { hours = "0" + hours; }
                    std::string minutes = std::to_string(now->tm_min);
                    if(now->tm_min < 10) { minutes = "0" + minutes; }
                    std::string time = hours + ":" + minutes;
                    messageList.addMessage(time, userName, messageContnet);
                }else{
                    userList.addMessage("", "", userName);
                }
            }

            ctx.drawRect(UserDivider, nullptr, 0);
            ctx.drawRect(InputDivider, nullptr, 0);
            messageList.drawMessages(ctx, windowColour, true);
            userList.drawMessages(ctx, windowColour, false);
            std::string inputText = input + "_";
            int maxXSize = 507 / ((ctx.getFontX()+1) * 2);
            if(inputText.length() > maxXSize) {
                int difference = inputText.length() - maxXSize;
                inputText.erase(0, difference);
                ctx.drawText(inputText, 0, 345, maxXSize, 1, 2, {255, 255, 255}, windowColour);
            }else{
                ctx.drawText(inputText, 0, 345, maxXSize, 1, 2, {255, 255, 255}, windowColour);
            }

            ctx.draw();
        }

    }

    SDL_StopTextInput();
    ctx.deInit();

	return 0;
}