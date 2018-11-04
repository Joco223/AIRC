#pragma once
#include <SDL.h>
#include <string>
#include <iostream>

class Rect;
class Line;
class Sprite;
class Text;
class SDL2_2D_Context;

struct Colour{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

class Text {
private:
	friend class SDL2_2D_Context;
	int posX, posY;
	int endX, endY;
	int scale;
	std::string text;
	Colour background, foreground;
	SDL_Texture* texture;

	void updateTexture(SDL2_2D_Context);
public:
	Text(std::string, int, int, int, Colour, Colour, SDL2_2D_Context);
	Text(std::string, int, int, int, int, int, Colour, Colour, SDL2_2D_Context);
	std::string getText();
	void setText(std::string, SDL2_2D_Context);
	void setTextPos(int, int, SDL2_2D_Context);
	void setTextSize(int, int, SDL2_2D_Context);
	void setScale(int, SDL2_2D_Context);
	void setForeground(Colour, SDL2_2D_Context);
	void setBackground(Colour, SDL2_2D_Context);
	int getPosX();
	int getPosY();
	int getEndX();
	int getEndY();
	int getSizeX(SDL2_2D_Context);
	int getSizeY(SDL2_2D_Context);
	int getScale();
	SDL_Texture* getTexture();
	void destroyTexture();
};

class Rect {
private:
	int posX, posY;
	int width, height;
	Colour colour;
public:
	Rect(int, int, int, int, Colour);
	void setPos(int, int);
	void setSize(int, int);
	void setColour(Colour);
	int getPosX();
	int getPosY();
	int getWidth();
	int getHeight();
	Colour getColour();
};

class Line {
private:
	int x1, y1;
	int x2, y2;
	Colour colour;
public:
	Line(int, int, int, int, Colour);
	void setEnd1(int, int);
	void setEnd2(int, int);
	void setColour(Colour);
	int getX1();
	int getX2();
	int getY1();
	int getY2();
	Colour getColour();
};

class Sprite {
private:
	friend class SDL2_2D_Context;
	SDL_Texture* texture;
public:
	Sprite(const char*, SDL2_2D_Context);
	SDL_Texture* getTexture();
};

class SDL2_2D_Context {
private:
	int width, height;
	int fontX, fontY;
	SDL_Window* window;
	SDL_Texture* font;
	Colour colour;
	SDL_Renderer* renderer;
public:
  
	SDL2_2D_Context();
	void Init(const char*, int, int, Colour);
	void deInit();
	void clear();
	void setWindowTitle(std::string);
	void loadFont(const char*, int, int);
	void draw();
	void drawText(Text);
	void drawText(std::string, int, int, int, Colour, Colour);
	void drawText(std::string, int, int, int, int, int, Colour, Colour);
	void drawRect(Rect, Sprite*, int);
	void drawRect(int, int, int, int, Colour);
	void drawLine(Line);
	void drawLine(int, int, int, int, Colour);

	SDL_Renderer* getRenderer();
	SDL_Texture* getFont();
	int getWidth();
	int getHeight();
	int getFontX();
	int getFontY();
	Colour getColour();
};
