#include <iostream>
#include "MineSweeperDataGatherer.h"
#include "MineSweeperHelper.h"

int main()
{
	DataGatherer gatherer;

	int show = 0;

	bool bot = false;

	bool foundMineSweeper = false;

	while (1)
	{
		if (gatherer.ObtainHandle())
		{
			if (GetForegroundWindow() == FindWindow(NULL, "Minesweeper"))
			{
				if (!foundMineSweeper)
				{
					system("cls");
					std::cout << "/////////////////////////////" << std::endl;
					std::cout << "//////BROUGHT TO YOU BY//////" << std::endl;
					std::cout << "//////////DEEZ NUTS//////////" << std::endl;
					std::cout << "////////////GOTEM////////////" << std::endl;
					std::cout << "/////////////////////////////" << std::endl;
					std::cout << "CONTROLS:\n";
					std::cout << "PRESS CONTROL FOR EZ AIM\n";
					std::cout << "PRESS UP ARROW KEY FOR GG EZ\n";
					std::cout << "PRESS DOWN ARROW KEY FOR HINTZ\n";
					foundMineSweeper = true;
				}

				if (GetAsyncKeyState(VK_CONTROL))
				{
					gatherer.ReadMinefield();
					POINT point = gatherer.ObtainClosestPoint();
					POINT screenPos = GetScreenPos();
					screenPos = CalculateOffset(screenPos, point.x, point.y);
					SlowMove(screenPos.x, screenPos.y);
					//SetCursorPos(screenPos.x, screenPos.y);
				}
				if (GetAsyncKeyState(VK_UP))
				{
					gatherer.ReadMinefield();
					gatherer.ShowAllBombs(show);
					Sleep(100);
					++show;
					if (show == 4)
						show = 0;
				}
				if (GetAsyncKeyState(VK_DOWN))
				{
					gatherer.ReadMinefield();
					POINT point = gatherer.ObtainClosestPoint();
					POINT screenPos = GetScreenPos();
					screenPos = CalculateOffset(screenPos, point.x, point.y);
					DrawLineToLocation(screenPos.x, screenPos.y);
					Sleep(100);
				}
				Sleep(5);
			}
			else
			{
				system("cls");
				std::cout << "Minesweeper is not in focus. Please click on minesweeper in order to gain focus\n";
				foundMineSweeper = false;
				Sleep(2500);
			}
		}
		else
		{
			system("cls");
			std::cout << "CANNOT FIND MINESWEEPER PROCESS. PLEASE RUN MINESWEEPER OR RESTART THE PROGRAM. THANK YOU CUM AGAIN\n";
			foundMineSweeper = false;
			Sleep(2500);
		}
		/*
		//Get the field
		POINT point = GetScreenPos();
		POINT cursorPos;
		GetCursorPos((LPPOINT)&cursorPos);
		SetCursorPos(point.x, point.y);
		system("pause");
		*/
	}

	return 0;
}