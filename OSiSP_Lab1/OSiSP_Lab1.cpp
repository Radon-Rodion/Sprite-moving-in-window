// OSiSP_Lab1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "OSiSP_Lab1.h"

#define MAX_LOADSTRING 100

#define WM_MENU_SPRITE_RECTANGLE 1233
#define WM_MENU_SPRITE_IMAGE 1232
#define WM_MENU_MOVEMENT_CONTROLLED 1223
#define WM_MENU_MOVEMENT_INERTABLE 1222

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

RECT clientRect;
Sprite* spritePtr;
const char MOVEMENT_KEYS[] = { 'S', 'W', 'A', 'D' };
const float BASE_SPEED = 3.0f;
const float BASE_HASTE = 0.5f;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC;
	switch (uMsg) {
	case WM_CREATE:
		createMenu(hWnd);
		spritePtr = prepareSprite(50, 30, 0x00FF3300, LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1)));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &clientRect);
		draw(spritePtr, hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
	{
		int dx = LOWORD(lParam) - spritePtr->xLeft;
		int dy = HIWORD(lParam) - spritePtr->yTop;
		if ((dx > 0) && (dx < spritePtr->xWidth) && (dy > 0) && (dy < spritePtr->yHeight))
			spritePtr->isMouseClicked = true;
		break;
	}
	case WM_MOUSEMOVE:
		if (spritePtr->isMouseClicked) {
			spritePtr->xLeft = LOWORD(lParam);
			spritePtr->yTop = HIWORD(lParam);
			preventOutscreen(spritePtr, clientRect);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_LBUTTONUP:
		spritePtr->isMouseClicked = false;
		break;
	case WM_TIMER:
		if (!spritePtr->isMouseClicked) {
			moveWithButtons(spritePtr, clientRect);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_MOUSEWHEEL:
	{
		short wheelDelta = HIWORD(wParam);
		if (LOWORD(wParam) == MK_SHIFT) {
			spritePtr->xLeft += wheelDelta * BASE_SPEED / 120;
		}
		else {
			spritePtr->yTop -= wheelDelta * BASE_SPEED / 120;
		}
		preventOutscreen(spritePtr, clientRect);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_COMMAND:
	{
		// Разобрать выбор в меню:
		switch (LOWORD(wParam))
		{
		case WM_MENU_SPRITE_RECTANGLE:
			//MessageBox(0, L"Поздравляю. Что делать будешь?", L"Custom", MB_OK);
			spritePtr->isSprite = true;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case WM_MENU_SPRITE_IMAGE:
			spritePtr->isSprite = false;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case WM_MENU_MOVEMENT_CONTROLLED:
			spritePtr->isInertable = false;
			spritePtr->xSpeed = spritePtr->ySpeed = 0;
			break;
		case WM_MENU_MOVEMENT_INERTABLE:
			spritePtr->isInertable = true;
			break;
		}
		break;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR  lpCmdLine, _In_ int  nCmdShow) {
	hInst = hInstance;
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;
	memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	hWnd = CreateWindowEx(0, L"MyWindowClass", L"Graphics", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 400, 450, NULL, NULL, hInstance, NULL);
	SetTimer(hWnd, 1234, 1000/10, NULL);

	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
	}

	return msg.wParam;
}



Sprite* prepareSprite(int width, int height, int colo, HBITMAP bitmap) {
	Sprite* spritePtr = new Sprite;
	spritePtr->xLeft = spritePtr->yTop = 0;
	spritePtr->xWidth = width;
	spritePtr->yHeight = height;
	spritePtr->isSprite = true;
	spritePtr->color = colo;
	spritePtr->texture = bitmap;
	GetObject(bitmap, sizeof(BITMAP), &(spritePtr->textureObj));
	spritePtr->memDC = NULL;
	spritePtr->isMouseClicked = false;
	spritePtr->isInertable = true;
	spritePtr->xSpeed = spritePtr->ySpeed = 0;
	return spritePtr;
}

void drawSprite(Sprite* spritePtr, HDC hDC) {
	if (spritePtr->memDC != NULL) spritePtr->memDC = NULL;
	RECT r;
	SetRect(&r, spritePtr->xLeft, spritePtr->yTop, spritePtr->xLeft+ spritePtr->xWidth, spritePtr->yTop+ spritePtr->yHeight);
	FillRect(hDC, &r, CreateSolidBrush(spritePtr->color));
}

void drawImage(Sprite* spritePtr, HDC hDC) {
	if(spritePtr->memDC == NULL)
		spritePtr->memDC = CreateCompatibleDC(hDC);
	SelectObject(spritePtr->memDC, spritePtr->texture);
	StretchBlt(hDC, spritePtr->xLeft, spritePtr->yTop, spritePtr->xWidth, spritePtr->yHeight, spritePtr->memDC, 0, 0
		, spritePtr->textureObj.bmWidth, spritePtr->textureObj.bmHeight,SRCCOPY);
}

void draw(Sprite* spritePtr, HDC hDC) {
	if (spritePtr->isSprite)
		drawSprite(spritePtr, hDC);
	else drawImage(spritePtr, hDC);
}

void moveWithButtons(Sprite* spritePtr, RECT area) {
	for (int i = 0; i < 4; i++) {
		if (GetKeyState(MOVEMENT_KEYS[i]) < 0) {
			int dX =  (i / 2) * ((2 * i % 4) - 1); //(0 when i == 0||1),( -1 when i == 2), (1 when i == 3)
			int dY = ((i / 2) - 1) * ((2 * i % 4) - 1); //(0 when i == 2||3),( -1 when i == 1), (1 when i == 0)

			if (spritePtr->isInertable) { //inertional movement - speed changing
				spritePtr->xSpeed += BASE_HASTE * dX;
				spritePtr->ySpeed += BASE_HASTE * dY;
			}
			else { //controlled movement
				spritePtr->xLeft += BASE_SPEED * dX;
				spritePtr->yTop += BASE_SPEED * dY;
			}
		}
	}
	if (spritePtr->isInertable) { //inertional movement - position changing
		if (detectXCollision(spritePtr, area))
			spritePtr->xSpeed *= -1;
		if (detectYCollision(spritePtr, area))
			spritePtr->ySpeed *= -1;
		spritePtr->xLeft += spritePtr->xSpeed;
		spritePtr->yTop += spritePtr->ySpeed;
	}
	preventOutscreen(spritePtr, area);
}

bool detectXCollision(Sprite* spritePtr, RECT area) {
	int dxLeft = spritePtr->xLeft - area.left;
	int dxRight = area.right - (spritePtr->xLeft + spritePtr->xWidth);
	if (dxLeft > 0 && dxRight > 0) return false;
		else return true;
}

bool detectYCollision(Sprite* spritePtr, RECT area) {
	int dyTop = spritePtr->yTop - area.top;
	int dyBot = area.bottom - (spritePtr->yTop + spritePtr->yHeight);
	if (dyTop > 0 && dyBot > 0) return false;
	else return true;
}

void preventOutscreen(Sprite* spritePtr, RECT area) {
	if (spritePtr->yTop < 0) spritePtr->yTop = 0;
	if (spritePtr->xLeft < 0) spritePtr->xLeft = 0;

	if (spritePtr->xLeft > area.right-spritePtr->xWidth) spritePtr->xLeft = area.right - spritePtr->xWidth;
	if (spritePtr->yTop > area.bottom-spritePtr->yHeight) spritePtr->yTop = area.bottom - spritePtr->yHeight;
}

void createMenu(HWND hWnd) {
	HMENU hMenuBar = CreateMenu();
	HMENU hSpriteOutlook = CreateMenu();
	HMENU hMovementType = CreateMenu();

	AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hSpriteOutlook, L"Sprite");
	AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hMovementType, L"Movement");

	AppendMenu(hSpriteOutlook, MF_STRING, WM_MENU_SPRITE_RECTANGLE, L"Rectangle");
	AppendMenu(hSpriteOutlook, MF_STRING, WM_MENU_SPRITE_IMAGE, L"Picture");

	AppendMenu(hMovementType, MF_STRING, WM_MENU_MOVEMENT_CONTROLLED, L"Controlled");
	AppendMenu(hMovementType, MF_STRING, WM_MENU_MOVEMENT_INERTABLE, L"Inertable");

	SetMenu(hWnd, hMenuBar);
}