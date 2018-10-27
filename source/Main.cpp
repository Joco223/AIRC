#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_Main.h>
#include <iostream>
#include <string>
#include <ctime>

#include "SDL2_2D.h"
#include "MessageList.h"

constexpr int w = 640, h = 360;
constexpr int scale = 2;
bool quit = false;

int main(int argc, char* argv[]) {

	Colour windowColour = {50, 50, 50};

    SDL2_2D_Context ctx;

    ctx.Init("AIRC", w, h, windowColour);

    SDL_Event e;

    Rect UserDivider(507, 0, 3, h, {255, 255, 255});
    Rect InputDivider(0, 341, 507, 3, {255, 255, 255});

    ctx.loadFont("Font.bmp", 5, 7);

    MessageList messageList(0, 0, 507, 341, scale);

    std::string input;
    std::string user = "Joco223";

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
                    break;
                case SDL_TEXTINPUT:
                    if(!(( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' ) && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) && SDL_GetModState() & KMOD_CTRL)) {
                        input += e.text.text;
                    }
                    break;
            }
            ctx.clear();

            ctx.drawRect(UserDivider, nullptr, 0);
            ctx.drawRect(InputDivider, nullptr, 0);
            messageList.drawMessages(ctx, windowColour);
            std::string inputText = input;
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