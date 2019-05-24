#define _SCL_SECURE_NO_WARNINGS
#define PIPES

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <string>

using namespace std;
void main(int argc, char* argv[])
{
	/*
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::string order;
	int choice = -1;
	while (choice != 0) {
		printf("Select genre:");
		printf("\n1.hip-hop\n2.rock\n3.electronic\n4.classical\n5.pop\n0.exit\n");
		scanf("%d", &choice);
		switch (choice) {
		case 1:
			order += "hip-hop ";
			break;
		case 2:
			order += "rock ";
			break;
		case 3:
			order += "electronic ";
			break;
		case 4:
			order += "classical ";
			break;
		case 5:
			order += "pop ";
			break;
		}
		system("CLS");
	}

	std::string cash;
	printf("Enter amount of money\n");
	std::cin >> cash;
	std::string path = "child.exe";

	//path += order;
	//path += cash;
	
	if (!CreateProcess(NULL,					// No module name (use command line)
		(TCHAR*)path.c_str(),				// Command line
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		NULL,		// lol
		NULL,					// Use parent's environment block
		NULL,					// Use parent's starting directory 
		&si,					// Pointer to STARTUPINFO structure
		&pi)					// Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		return -1;
	}
	*/

	int number = atoi(argv[1]);	
	char EventID[10];	
	snprintf(EventID, 10, " %dc", number);
	
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, EventID);	//закрытие клиента контроллером
	HANDLE print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Print");		//клиент разрешает серверу печатать сообщение
	HANDLE isFree = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "IsFree");	//сервер сообщает клиенту, что свободен
	HANDLE channel;															//канал
	HANDLE massiv[2] = { isFree, close };

	char buf[40];
	snprintf(buf, 40, "Process number %d\n", number);	
	while (true)
	{
		if (WaitForSingleObject(close, 0) == WAIT_OBJECT_0)
		{
			CloseHandle(close);
			CloseHandle(print);
			CloseHandle(isFree);
			CloseHandle(channel);
			return;
		}

		if (WaitForMultipleObjects(2, massiv, FALSE, INFINITE) == WAIT_OBJECT_0 + 1)	
		{
			CloseHandle(close);
			CloseHandle(print);
			CloseHandle(isFree);
			CloseHandle(channel);
			return;
		}
		else
		{	
			WaitNamedPipe("\\\\.\\pipe\\Pipe", INFINITE);	
			channel = CreateFile(TEXT("\\\\.\\pipe\\Pipe"),
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (channel == INVALID_HANDLE_VALUE)
			{
				cout << "Channel couldn't be opened" << endl;
			}

			WriteFile(channel, buf, strlen(buf), NULL, NULL);
			ReleaseSemaphore(print, 1, NULL);
			CloseHandle(channel);
			//ReleaseSemaphore(isFree, 1, NULL);
		}
	}
}