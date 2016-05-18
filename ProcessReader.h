#pragma once

#include <Windows.h>

class ProcessReader
{
private:
	HWND windowHandleToProcess;
	HANDLE handleToProcess;
public:
	ProcessReader() {}
	~ProcessReader() {}
	HWND GetWindowHandleToProcess() const { return windowHandleToProcess; }
	HANDLE GetHandleToProcess() const { return handleToProcess; }
	int GrabProcess();

	template<typename T>
	inline T readFromMemory(LPVOID address)
	{
		T value;
		ReadProcessMemory(this->handleToProcess, address, &value, sizeof(value), NULL);
		return value;
	}

	template<typename T>
	inline void writeToMemory(LPVOID address, T value)
	{
		WriteProcessMemory(this->handleToProcess, address, &value, sizeof(value), NULL);
	}
};

/*
returns 1 if sucessful
*/

int ProcessReader::GrabProcess()
{
	HWND handle = FindWindow(NULL, "Minesweeper");
	if (handle)
	{
		DWORD PID;
		if (GetWindowThreadProcessId(handle, &PID))
		{
			HANDLE handleToProcess = OpenProcess(PROCESS_VM_OPERATION |
				PROCESS_VM_READ |
				PROCESS_VM_WRITE |
				PROCESS_CREATE_THREAD, FALSE, PID);
			if (handleToProcess)
			{
				this->windowHandleToProcess = handle;
				this->handleToProcess = handleToProcess;
				return 1;
			}
		}
	}
	return 0;
}