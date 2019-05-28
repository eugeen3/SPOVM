#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <ctime>

#define SERVER_PATH "Server.exe"

using namespace std;

void PrintLastError()
{
	printf("Create Process failed (%d)\n", GetLastError());
	_getch();
	return;
}

PROCESS_INFORMATION CreateNewProcess(char* path, char *cmdline)
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION pi;
	if (!CreateProcess(path, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
		PrintLastError();
	return pi;
}

DWORD WINAPI myThread(PVOID params)
{
	int number = atoi((char*)params);
	char EventID[10];
	HANDLE conn;

	snprintf(EventID, 10, " %dc", number);
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, EventID);
	HANDLE isFree = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "IsFree");
	HANDLE print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Print");
	HANDLE massiv[2] = { isFree, close };

	char buf[40];
	snprintf(buf, 255, "Thread number %d\n\0", number);

	while (true)
	{
		if (WaitForSingleObject(close, 0) == WAIT_OBJECT_0 ||
			(WaitForMultipleObjects(2, massiv, FALSE, INFINITE) == WAIT_OBJECT_0 + 1))
		{
			CloseHandle(close);
			CloseHandle(isFree);
			CloseHandle(print);
			CloseHandle(conn);
			return 0;
		}
		else
		{
			WaitNamedPipe("\\\\.\\pipe\\Pipe", INFINITE);
			conn = CreateFile(TEXT("\\\\.\\pipe\\Pipe"),
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (conn == INVALID_HANDLE_VALUE) cout << "Oops. Channel couldn't be opened" << endl;;
			WriteFile(conn, buf, strlen(buf), NULL, NULL);
			CloseHandle(conn);
			ReleaseMutex(isFree);
			ReleaseSemaphore(print, 1, NULL);
		}
	}
	return 0;
}


int main()
{
	HANDLE closeServer = CreateEvent(NULL, TRUE, FALSE, "CloseServer");
	HANDLE print = CreateSemaphore(NULL, 0, 1, "Print");
	HANDLE isFree = CreateMutex(NULL, FALSE, "IsFree");

	vector<HANDLE> closeProc;
	vector<HANDLE> procInf;

	char EventID[10];
	char key = 0;
	PROCESS_INFORMATION server = CreateNewProcess(SERVER_PATH, NULL);
	while (true)
	{
		if (_kbhit())
		{
			key = _getch();
			switch (key)
			{
			case '+':
				snprintf(EventID, 10, " %dc", closeProc.size());
				closeProc.push_back(CreateEvent(NULL, TRUE, FALSE, EventID));
				procInf.push_back(CreateThread(NULL, 0, myThread, EventID, 0, 0));
				break;
			case '-':
				if (closeProc.size() > 0)
				{
					SetEvent(closeProc.back());
					WaitForSingleObject(procInf.back(), INFINITE);
					CloseHandle(procInf.back());
					CloseHandle(closeProc.back());
					closeProc.pop_back();
					procInf.pop_back();
				}
				break;
			case 'q':
				while (procInf.size() > 0)
				{
					SetEvent(closeProc.back());
					WaitForSingleObject(procInf.back(), INFINITE);
					CloseHandle(procInf.back());
					CloseHandle(closeProc.back());
					closeProc.pop_back();
					procInf.pop_back();
				}
				SetEvent(closeServer);
				WaitForSingleObject(server.hProcess, INFINITE);
				CloseHandle(server.hProcess);
				CloseHandle(server.hThread);
				CloseHandle(closeServer);
				return 0;
			}
		}
	}
	return 0;
}