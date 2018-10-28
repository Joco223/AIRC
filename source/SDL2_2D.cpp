#include "SDL2_2D.h"

//SDL2_2D_CONTEXT

SDL2_2D_Context::SDL2_2D_Context()
        :
        window(nullptr),
        renderer(nullptr),
        width(0),
        height(0) {};

void SDL2_2D_Context::Init(const char* window_name, int window_width, int window_height, Colour newColour) {
    colour = newColour;
    window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    width = window_width;
    height = window_height;
};

void SDL2_2D_Context::deInit() {
    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
};

void SDL2_2D_Context::clear() {
    SDL_RenderClear(renderer);
}

SDL_Renderer* SDL2_2D_Context::getRenderer() {
    return renderer;
}

void SDL2_2D_Context::loadFont(const char* font_path, int fontX_, int fontY_) {
    fontX = fontX_;
    fontY = fontY_;
    SDL_Surface* temp = SDL_LoadBMP(font_path);
    SDL_SetColorKey(temp, SDL_TRUE, 0);
    font = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
}

void SDL2_2D_Context::draw() {
    SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, 255);
    SDL_RenderPresent(renderer);
};

void SDL2_2D_Context::drawText(Text text) {
    SDL_Rect temp = {text.getPosX(), text.getPosY(), text.getEndX() * (fontX+1) * text.getScale(), text.getEndY() * (fontY+1) * text.getScale()};
    SDL_RenderCopyEx(renderer, text.getTexture(), nullptr, &temp, 0, nullptr, SDL_FLIP_NONE);
}

void SDL2_2D_Context::drawText(std::string text, int posX, int posY, int scale, Colour foreground, Colour background) {
    for(int i = 0; i < text.length(); i++) {
        int texPosX = ((unsigned int)text[i] - 32) * fontX;
        SDL_Rect src = {texPosX, 0, fontX, fontY};
        SDL_Rect bck = {0, 0, fontX, fontY};
        SDL_Rect dst = {posX + i*(fontX+1)*scale, posY, fontX*scale, fontY*scale};
        SDL_Rect ds2 = {posX + i*(fontX+1)*scale, posY, (fontX+1)*scale, (fontY+1)*scale};
        SDL_SetTextureColorMod(font, background.red, background.green, background.blue);
        SDL_RenderCopyEx(renderer, font, &bck, &ds2, 0, nullptr, SDL_FLIP_NONE);
        if(texPosX != 0) {
            SDL_SetTextureColorMod(font, foreground.red, foreground.green, foreground.blue);
            SDL_RenderCopyEx(renderer, font, &src, &dst, 0, nullptr, SDL_FLIP_NONE);
        }       
    }
}

void SDL2_2D_Context::drawText(std::string text, int posX, int posY, int endX, int endY, int scale, Colour foreground, Colour background) {
    int xOffset = 0;
    int yOffset = 0;
    for(int i = 0; i < text.length(); i++) {
        if((yOffset+1)*(fontY+1)*scale < posY + endY*(fontY+1)*scale) {
            int texPosX = ((unsigned int)text[i] - 32) * fontX;
            SDL_Rect src = {texPosX, 0, fontX, fontY};
            SDL_Rect bck = {0, 0, fontX, fontY};
            SDL_Rect dst = {posX, posY, fontX*scale, fontY*scale};
            SDL_Rect ds2 = {posX, posY, (fontX+1)*scale, (fontY+1)*scale};

            dst.y += yOffset*(fontY+1)*scale;
            ds2.y += yOffset*(fontY+1)*scale;

            dst.x += xOffset*(fontX+1)*scale;
            ds2.x += xOffset*(fontX+1)*scale;

            if(posX + (xOffset + 2)*(fontX+1)*scale > posX + (endX*(fontX+1)*scale)) {
                yOffset++;
                xOffset = 0;
            }else{        
                xOffset++;
            }

            SDL_SetTextureColorMod(font, background.red, background.green, background.blue);
            SDL_RenderCopyEx(renderer, font, &bck, &ds2, 0, nullptr, SDL_FLIP_NONE);
            if(texPosX != 0) {
                SDL_SetTextureColorMod(font, foreground.red, foreground.green, foreground.blue);
                SDL_RenderCopyEx(renderer, font, &src, &dst, 0, nullptr, SDL_FLIP_NONE);
            } 
        }else{
            break;
        }            
    }
}

void SDL2_2D_Context::drawRect(Rect rect, Sprite* sprite, int angle){
    if(sprite == nullptr){
        SDL_SetRenderDrawColor(renderer, rect.getColour().red, rect.getColour().green, rect.getColour().blue, 255);
        SDL_Rect temp;
        temp.x = rect.getPosX();
        temp.y = rect.getPosY();
        temp.w = rect.getWidth();
        temp.h = rect.getHeight();
        SDL_RenderFillRect(renderer, &temp);
    }else{
        int tile_width, tile_height;
		SDL_QueryTexture(sprite->getTexture(), nullptr, nullptr, &tile_width, &tile_height);

        SDL_Rect src = {0, 0, tile_width, tile_height};
        SDL_Rect dest = {rect.getPosX(), rect.getPosY(), rect.getWidth(), rect.getHeight()};
        SDL_Point center = {tile_width/2, tile_height/2};

        SDL_RenderCopyEx(renderer, sprite->getTexture(), &src, &dest, angle, &center, SDL_FLIP_NONE);
    }
};

void SDL2_2D_Context::drawRect(int posX, int posY, int width, int height, Colour colour){
    SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, 255 );
    SDL_Rect temp;
    temp.x = posX;
    temp.y = posY;
    temp.w = width;
    temp.h = height;
    SDL_RenderFillRect(renderer, &temp);
};

void SDL2_2D_Context::drawLine(Line line){
    SDL_SetRenderDrawColor(renderer, line.getColour().red, line.getColour().green, line.getColour().blue, 255);
    SDL_RenderDrawLine(renderer, line.getX1(), line.getY1(), line.getX2(), line.getY2());
};

void SDL2_2D_Context::drawLine(int x1, int y1, int x2, int y2, Colour colour){
    SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, 255);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
};

void SDL2_2D_Context::setWindowTitle(std::string newTitle) {
    SDL_SetWindowTitle(window, newTitle.c_str());
}

SDL_Texture* SDL2_2D_Context::getFont() { return font; }
int SDL2_2D_Context::getWidth() { return width; }
int SDL2_2D_Context::getHeight() { return height; }
int SDL2_2D_Context::getFontX() { return fontX; }
int SDL2_2D_Context::getFontY() { return fontY; }
Colour SDL2_2D_Context::getColour() { return colour; }

//TEXT

Text::Text(std::string text_, int posX_, int posY_, int scale_, Colour foreground_, Colour background_, SDL2_2D_Context ctx)
     :
     text(text_),
     posX(posX_),
     posY(posY_),
     endY(1),
     scale(scale_),
     foreground(foreground_),
     background(background_)
     {
        endX = text.length();
        updateTexture(ctx);
     };

Text::Text(std::string text_, int posX_, int posY_, int endX_, int endY_, int scale_, Colour foreground_, Colour background_, SDL2_2D_Context ctx)
     :
     text(text_),
     posX(posX_),
     posY(posY_),
     endX(endX_),
     endY(endY_),
     scale(scale_),
     foreground(foreground_),
     background(background_)
     {
        updateTexture(ctx);
     };

std::string Text::getText() { return text; }

void Text::updateTexture(SDL2_2D_Context ctx) {
    texture = SDL_CreateTexture(ctx.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, endX*(ctx.getFontX()+1)*scale, endY*(ctx.getFontY()+1)*scale);

    SDL_SetRenderTarget(ctx.getRenderer(), texture);

    ctx.drawRect(0, 0, endX*(ctx.getFontX()+1)*scale, endY*(ctx.getFontY()+1)*scale, background);

    int xOffset = 0;
    int yOffset = 0;
    for(int i = 0; i < text.length(); i++) {
        if((yOffset+1)*(ctx.getFontY()+1)*scale <= 0 + endY*(ctx.getFontY()+1)*scale) {
            int texPosX = ((unsigned int)text[i] - 32) * ctx.getFontX();
            SDL_Rect src = {texPosX, 0, ctx.getFontX(), ctx.getFontY()};
            SDL_Rect bck = {0, 0, ctx.getFontX(), ctx.getFontY()};
            SDL_Rect dst = {0, 0, ctx.getFontX()*scale, ctx.getFontY()*scale};
            SDL_Rect ds2 = {0, 0, (ctx.getFontX()+1)*scale, (ctx.getFontY()+1)*scale};

            dst.y += yOffset*(ctx.getFontY()+1)*scale;
            ds2.y += yOffset*(ctx.getFontY()+1)*scale;

            dst.x += xOffset*(ctx.getFontX()+1)*scale;
            ds2.x += xOffset*(ctx.getFontX()+1)*scale;

            if(0 + (xOffset + 2)*(ctx.getFontX()+1)*scale > 0 + (endX*(ctx.getFontX()+1)*scale)) {
                yOffset++;
                xOffset = 0;
            }else{        
                xOffset++;
            }

            SDL_SetTextureColorMod(ctx.getFont(), background.red, background.green, background.blue);
            SDL_RenderCopyEx(ctx.getRenderer(), ctx.getFont(), &bck, &ds2, 0, nullptr, SDL_FLIP_NONE);
            if(texPosX != 0) {
                SDL_SetTextureColorMod(ctx.getFont(), foreground.red, foreground.green, foreground.blue);
                SDL_RenderCopyEx(ctx.getRenderer(), ctx.getFont(), &src, &dst, 0, nullptr, SDL_FLIP_NONE);
            } 
        }else{
            break;
        }            
    }

    SDL_SetRenderTarget(ctx.getRenderer(), nullptr);
}

void Text::setText(std::string text_, SDL2_2D_Context ctx) {
    text = text_;
    updateTexture(ctx);
}

void Text::setTextPos(int posX_, int posY_, SDL2_2D_Context ctx) {
    posX = posX_;
    posY = posY_;
    updateTexture(ctx);
}

void Text::setTextSize(int endX_, int endY_, SDL2_2D_Context ctx) {
    endX = endX_;
    endY = endY_;
    updateTexture(ctx);
}

void Text::setScale(int scale_, SDL2_2D_Context ctx) {
    scale = scale_;
    updateTexture(ctx);
}

void Text::setBackground(Colour background_, SDL2_2D_Context ctx) {
    background = background_;
    updateTexture(ctx);
}

void Text::setForeground(Colour foreground_, SDL2_2D_Context ctx) {
    foreground = foreground_;
    updateTexture(ctx);
}

int Text::getPosX() { return posX; }
int Text::getPosY() { return posY; }
int Text::getEndX() { return endX; }
int Text::getEndY() { return endY; }
int Text::getSizeX(SDL2_2D_Context ctx) { return endX*(ctx.getFontX()+1)*scale; }
int Text::getSizeY(SDL2_2D_Context ctx) { return endY*(ctx.getFontY()+1)*scale; }
int Text::getScale() { return scale; }
SDL_Texture* Text::getTexture() { return texture; }

//RECT

Rect::Rect(int posX_, int posY_, int width_, int height_, Colour c)
     :
     posX(posX_),
     posY(posY_),
     width(width_),
     height(height_),
     colour(c)
     {};

void Rect::setColour(Colour newColour){ colour = newColour; };
void Rect::setPos(int posX_, int posY_) { posX = posX_; posY = posY_; };
void Rect::setSize(int width_, int height_) { width = width_; height = height_; }

int Rect::getPosX() { return posX; }
int Rect::getPosY() { return posY; }
int Rect::getWidth() { return width; }
int Rect::getHeight() { return height; }
Colour Rect::getColour() { return colour; }

//LINE

Line::Line(int x1_, int y1_, int x2_, int y2_, Colour c)
     :
     x1(x1_),
     y1(y1_),
     x2(x2_),
     y2(y2_),
     colour(c)
     {};

void Line::setColour(Colour newColour) { colour = newColour; };
void Line::setEnd1(int x1_, int y1_) { x1 = x1_; y1 = y1_; };
void Line::setEnd2(int x2_, int y2_) { x2 = x2_; y2 = y2_; };

int Line::getX1() { return x1; }
int Line::getX2() { return x2; }
int Line::getY1() { return y1; }
int Line::getY2() { return y2; }
Colour Line::getColour() { return colour; }

//SPRITE

Sprite::Sprite(const char* texture_path, SDL2_2D_Context ctx) {
    SDL_Surface* temp = SDL_LoadBMP(texture_path);
    SDL_SetColorKey(temp, SDL_TRUE, 0);
    texture = SDL_CreateTextureFromSurface(ctx.getRenderer(), temp);
    SDL_FreeSurface(temp);
}

SDL_Texture* Sprite::getTexture() { return texture; }
