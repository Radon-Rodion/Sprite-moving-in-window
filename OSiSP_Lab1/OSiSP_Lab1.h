#pragma once

#include "resource.h"
void paintWindows(HDC hDC, HBRUSH light, HBRUSH dark);

struct Sprite {
	int xLeft, yTop;
	int xWidth, yHeight;
	bool isSprite;
	bool isMouseClicked;
	bool isInertable;
	float xSpeed, ySpeed;

	int color;

	HBITMAP texture;
	BITMAP textureObj;
	HDC memDC;
};
typedef struct Sprite Sprite;

Sprite* prepareSprite(int width, int height, int colo, HBITMAP bitmap);

//drawing
void drawSprite(Sprite* spritePtr, HDC hDC);
void drawImage(Sprite* spritePtr, HDC hDC);
void draw(Sprite* spritePtr, HDC hDC);

//moving
void moveWithButtons(Sprite* spritePtr, RECT area);
bool detectXCollision(Sprite* spritePtr, RECT area);
bool detectYCollision(Sprite* spritePtr, RECT area);
void preventOutscreen(Sprite* spritePtr, RECT area);

//changing modes
void createMenu(HWND hWnd);
