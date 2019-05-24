#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[])
{
	int number = atoi(argv[1]);
	char EventID[10];
	snprintf(EventID, 10, " %d", number);
	HANDLE print = OpenEvent(EVENT_ALL_ACCESS, FALSE, EventID);

	snprintf(EventID, 10, " %dc", number);
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, EventID);


	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::string path = "child.exe";
	
	if (!CreateProcess(NULL, (TCHAR*)path.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		return -1;
	}

	char buf[255];
	long int PK = number;
	snprintf(buf, 255, "PK: %d. Process %d\n\0", PK, number);
	while (true)
	{
		if (WaitForSingleObject(print, 1) == WAIT_OBJECT_0)
		{
			for (int i = 0; i < strlen(buf); i++)
			{
				printf("%c", buf[i]);
				Sleep(50);
			}
			ResetEvent(print);
		}

		if (WaitForSingleObject(close, 1) == WAIT_OBJECT_0)
		{
			CloseHandle(close);
			CloseHandle(print);
			return 0;
		}
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);

	return 0;
}