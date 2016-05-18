#pragma once

#include <Windows.h>
#include <math.h>
#include <iostream>

POINT CalculateOffset(POINT cursorPos, int xpos, int ypos)
{
	cursorPos.x += xpos * 16;
	cursorPos.y += ypos * 16;
	return cursorPos;
}

POINT GetScreenPos()
{
	RECT rect;
	HWND handle = FindWindow(NULL, "Minesweeper");
	GetClientRect(handle, (LPRECT)&rect);
	POINT point;
	point.x = rect.left;
	point.y = rect.top;
	ClientToScreen(handle, (LPPOINT)&point); 
	// 12 56 + 8 8 offsets
	point.x += 12 + 8;
	point.y += 56 + 8;
	return point;
}

void SlowMove(int target_x, int target_y)
{
	POINT cursorPos;
	GetCursorPos((LPPOINT)&cursorPos);
	if (target_x - cursorPos.x < 0)
	{
		SetCursorPos(cursorPos.x - 1, cursorPos.y);
	}
	else if(target_x - cursorPos.x > 0)
	{
		SetCursorPos(cursorPos.x + 1, cursorPos.y);
	}

	if(target_y - cursorPos.y < 0)
	{
		SetCursorPos(cursorPos.x, cursorPos.y - 1);
	}
	else if(target_y - cursorPos.y > 0)
	{
		SetCursorPos(cursorPos.x, cursorPos.y + 1);
	}

	if((target_x == cursorPos.x) && (target_y == cursorPos.y))
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}

HBITMAP CaptureScreen(int x, int y, int w, int h)
{
	HDC screenDC = GetDC(0);
	HDC screenMemoryDC = CreateCompatibleDC(screenDC);

	int capX = GetDeviceCaps(screenDC, HORZRES);
	int capY = GetDeviceCaps(screenDC, VERTRES);

	HBITMAP hBitmap = CreateCompatibleBitmap(screenDC, w, h);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(screenMemoryDC, hBitmap);

	BitBlt(screenMemoryDC, 0, 0, w, h, screenDC, x, y, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(screenMemoryDC, hBitmapOld);
	ReleaseDC(0, screenDC);
	DeleteDC(screenMemoryDC);
	return hBitmap;
}

void DrawScreen(int x, int y, HBITMAP hBitmap)
{
	HDC screenDC = GetDC(0);
	HDC screenMemoryDC = CreateCompatibleDC(screenDC);

	HBITMAP hBitmapOld = (HBITMAP)SelectObject(screenMemoryDC, hBitmap);

	BITMAP bitmap;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);

	BitBlt(screenDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, screenMemoryDC, x, y, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(screenMemoryDC, hBitmapOld);
	ReleaseDC(0, screenDC);
	DeleteDC(screenMemoryDC);
}

void DrawLineToLocation(int target_x, int target_y)
{
	HDC screenDC = GetDC(0);

	HPEN hPenOld;
	HPEN hLinePen;

	hLinePen = CreatePen(PS_SOLID, 15, RGB(0, 255, 125));
	hPenOld = (HPEN)SelectObject(screenDC, hLinePen);

	POINT cursorPos;
	GetCursorPos((LPPOINT)&cursorPos);
	MoveToEx(screenDC, cursorPos.x, cursorPos.y, NULL);
	LineTo(screenDC, target_x, target_y);
	SelectObject(screenDC, hPenOld);
	DeleteObject(hLinePen);
}