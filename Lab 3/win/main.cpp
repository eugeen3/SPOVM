#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <vector>

#define SERVER_PATH "client.exe"
#define CLIENT_PATH "server.exe"

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
	if (!CreateProcess(path, cmdline, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
		PrintLastError();
	return pi;
}

int main()
{
	vector<PROCESS_INFORMATION> procArray;
	vector<HANDLE> closeClientsEvents;

	PROCESS_INFORMATION serverPI = CreateNewProcess(SERVER_PATH, NULL);
	HANDLE closeServer = CreateEvent(NULL, TRUE, FALSE, "CloseServer");

	char key; 
	while (true)
	{
		if (_kbhit())	
		{
			key = _getch();	
			switch (key)
			{
			case '+':
				char id[10];
				snprintf(id, 10, " %dc", closeClientsEvents.size());					
				closeClientsEvents.push_back(CreateEvent(NULL, TRUE, FALSE, id));	
				snprintf(id, 10, " %d", procArray.size());				
				procArray.push_back(CreateNewProcess(CLIENT_PATH, id));
				break;
			case '-':
				if (closeClientsEvents.size() > 0)
				{
					SetEvent(closeClientsEvents.back());	
					WaitForSingleObject(procArray.back().hProcess, INFINITE);	
					CloseHandle(closeClientsEvents.back());	
					CloseHandle(procArray.back().hProcess);
					CloseHandle(procArray.back().hThread);
					closeClientsEvents.pop_back();
					procArray.pop_back();
				}
				break;
			case 'q':
				while (procArray.size() > 0) {
					SetEvent(closeClientsEvents.back());	
					WaitForSingleObject(procArray.back().hProcess, INFINITE);	
					CloseHandle(closeClientsEvents.back());	
					CloseHandle(procArray.back().hProcess);
					CloseHandle(procArray.back().hThread);
					closeClientsEvents.pop_back();
					procArray.pop_back();
				}
				SetEvent(closeServer);
				WaitForSingleObject(serverPI.hProcess, INFINITE);
				CloseHandle(serverPI.hProcess);
				CloseHandle(serverPI.hThread);
				CloseHandle(closeServer);
				return 0;
			}
		}
	}
	return 0;
}