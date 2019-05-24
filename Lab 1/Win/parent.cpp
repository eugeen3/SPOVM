#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <tchar.h>

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char* order = (char*)calloc(1, sizeof(char));
	int choice = -1;
	while (choice != 0) {
		printf("Select genre:");
		printf("\n1.hip-hop\n2.rock\n3.electronic\n4.classical\n5.pop\n0.exit\n");
		scanf("%d", &choice);
		switch (choice) {
		case 1:
			strcat(order, "hip-hop ");
			break;
		case 2:
			strcat(order, "rock ");
			break;
		case 3:
			strcat(order, "electronic ");
			break;
		case 4:
			strcat(order, "classical ");
			break;
		case 5:
			strcat(order, "pop ");
			break;
		}
		//system("CLS");
	}

	char cash[10];
	printf("Enter amount of money\n");
	scanf("%s", cash);

	char path[200] = "child.exe ";

	strcat(path, order);
	strcat(path, cash);

	if (!CreateProcess(NULL,					// No module name (use command line)
		(TCHAR*)path,				// Command line
		NULL,					// Process handle not inheritable
		NULL,					// Thread handle not inheritable
		FALSE,					// Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,		// lol
		NULL,					// Use parent's environment block
		NULL,					// Use parent's starting directory 
		&si,					// Pointer to STARTUPINFO structure
		&pi)					// Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		return -1;
	}

	SYSTEMTIME lt;

	while (WaitForSingleObject(pi.hProcess, 50))
	{
		GetLocalTime(&lt);
		printf("%02d:%02d:%02d p\r", lt.wHour, lt.wMinute, lt.wSecond);
	}

	DWORD excode;
	GetExitCodeProcess(pi.hProcess, &excode);

	switch (excode) {
	case 0:
		printf("\nPurchase was made succesfully!\n");
		break;
	case 1:
		printf("\nNot enough albums in store\n");
		break;
	case 2:
		printf("\nNot enough money\n");
		break;
	}

	CloseHandle(pi.hProcess);
	free(order);
	system("pause");
	return 0;
}