#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_Main.h>
#include <iostream>
#include <string>

#include "SDL2_2D.h"

constexpr int w = 640, h = 360;
bool quit = false;

int main(int argc, char* argv[]) {

	Colour windowColour = {50, 50, 50};

    SDL2_2D_Context ctx;

    ctx.Init("AIRC", w, h, windowColour);

    SDL_Event event;

    Rect UserDivider(507, 0, 3, h, {255, 255, 255});
    Rect InputDivider(0, 333, 507, 3, {255, 255, 255});

    ctx.loadFont("Font.bmp", 5, 7);

    Text test("Is this working?", 0, 0, 2, {255, 255, 255}, windowColour, ctx);
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            }
        }

        ctx.clear();

        ctx.drawRect(UserDivider, nullptr, 0);
        ctx.drawRect(InputDivider, nullptr, 0);
        ctx.drawText(test);

        ctx.draw();

    }

    ctx.deInit();

	return 0;
}