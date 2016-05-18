#pragma once

#include "ProcessReader.h"
#include <iostream>
#include <math.h>

enum TYPE{SHOWN_MINE, HIDDEN_MINE, VALUE, UNKNOWN};

//starts at 0x0100533A
struct mineSize 
{
	int width = 0;
	int height = 0;
};

struct mine
{
	int xpos;
	int ypos;
	TYPE type;
	LPVOID address;
};

class DataGatherer
{
private:
	ProcessReader reader;
	struct mine* minefield;
	struct mineSize size;
public:
	DataGatherer() {}
	~DataGatherer() { if (minefield != NULL) delete[] minefield; }
	int ObtainHandle();
	void ReadMinefield();
	void ShowAllBombs(int);
	POINT ObtainClosestPoint();
};

int DataGatherer::ObtainHandle()
{
	if (this->reader.GrabProcess())
	{
		return 1;
	}
	return 0;
}

void DataGatherer::ReadMinefield()
{
	this->size = this->reader.readFromMemory<struct mineSize>((LPVOID)0x01005334);
	this->minefield = new struct mine[size.width * size.height]();

	//read the process and gather the spots
	//std::cout << this->size.width << " | " << this->size.height << "           " << std::hex << 0x01005340 + this->size.width << std::endl;

	int index = 0;

	int x = 0;
	int y = -1;
	
	bool read = 0;

	int offset = 0;


	while (index != this->size.width * this->size.height)
	{
		LPVOID address = (LPVOID)(0x01005340 + this->size.width + offset + 2);
		BYTE byte = this->reader.readFromMemory<BYTE>(address);

		if (byte == 0x10)
		{
			if (read == 0)
			{
				read = 1;
				x = 0;
				y++;
			}
			else
			{
				read = 0;
			}
		}
		else if (read == 1)
		{
			this->minefield[index].xpos = x;
			this->minefield[index].ypos = y;
			this->minefield[index].address = address;
			switch (byte)
			{
			case 0x0F: this->minefield[index].type = UNKNOWN; break;
			case 0x8F: this->minefield[index].type = HIDDEN_MINE; break;
			case 0x8A: this->minefield[index].type = HIDDEN_MINE; break;
			case 0x8D: this->minefield[index].type = HIDDEN_MINE; break;
			case 0x8E: this->minefield[index].type = HIDDEN_MINE; break;
			default: this->minefield[index].type = VALUE; break;
			}
			//std::cout << +byte << std::endl;
			index++;
			x++;
		}
		++offset;
	}
}

double DistanceFormula(HWND handle, int x, int y)
{
	POINT cursorPos;
	GetCursorPos((LPPOINT)&cursorPos);
	RECT rect;
	GetClientRect(handle, (LPRECT)&rect);
	POINT point;
	point.x = rect.left;
	point.y = rect.top;
	ClientToScreen(handle, (LPPOINT)&point);
	point.x += 20;
	point.y += 64;
	return sqrt((point.x + x * 16 - cursorPos.x) * (point.x + x * 16 - cursorPos.x) + (point.y + y * 16 - cursorPos.y) * (point.y + y * 16 - cursorPos.y));
}

POINT DataGatherer::ObtainClosestPoint()
{
	int index = 0;
	POINT location;
	location.x = 0;
	location.y = 0;
	double lowestDistance = DBL_MAX;
	while (index != this->size.width * this->size.height)
	{
		if (this->minefield[index].type == UNKNOWN)
		{
			if (DistanceFormula(this->reader.GetWindowHandleToProcess(), this->minefield[index].xpos, this->minefield[index].ypos) < lowestDistance)
			{
				lowestDistance = DistanceFormula(this->reader.GetWindowHandleToProcess(), this->minefield[index].xpos, this->minefield[index].ypos);
				location.x = this->minefield[index].xpos;
				location.y = this->minefield[index].ypos;
			}
		}
		++index;
	}
	return location;
}

void DataGatherer::ShowAllBombs(int show)
{
	int index = 0;
	while (index != this->size.width * this->size.height)
	{
		if (this->minefield[index].type == HIDDEN_MINE && show == 0)
		{
			this->reader.writeToMemory<char>(this->minefield[index].address, 0x8A);
			//std::cout << +this->reader.readFromMemory<char>(this->minefield[index].address) << "\n";
		}
		else if (this->minefield[index].type == HIDDEN_MINE && show == 1)
		{
			this->reader.writeToMemory<char>(this->minefield[index].address, 0x8D);
		}
		else if (this->minefield[index].type == HIDDEN_MINE && show == 2)
		{
			this->reader.writeToMemory<char>(this->minefield[index].address, 0x8E);
		}
		else if (this->minefield[index].type == HIDDEN_MINE && show == 3)
		{
			this->reader.writeToMemory<char>(this->minefield[index].address, 0x8F);
		}
		++index;
	}

	//Code Cave -> draws everything
	BYTE codeCave[8] =
	{
		0xB8, 0x2E, 0x27, 0x00, 0x01, //Call draw function
		0xFF, 0xD0, //Call eax
		0xC3 //return
	};

	auto remoteCave = VirtualAllocEx(this->reader.GetHandleToProcess(), 0, sizeof(codeCave), MEM_COMMIT, PAGE_EXECUTE);
	
	WriteProcessMemory(this->reader.GetHandleToProcess(), remoteCave, codeCave, sizeof(codeCave), NULL);

	//Use Thread injection to inject cave
	HANDLE thread = CreateRemoteThread(this->reader.GetHandleToProcess(), NULL, NULL, (LPTHREAD_START_ROUTINE)remoteCave, NULL, NULL, NULL);

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);

	VirtualFreeEx(this->reader.GetHandleToProcess(), remoteCave, sizeof(codeCave), MEM_FREE);
}

