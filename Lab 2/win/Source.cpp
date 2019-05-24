#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <ctime>

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

int main(int argc, char* argv[])
{
	bool flag = false;
	int procCounter = 0;
	int count = 0;
	char path[] = "parent.exe";
	vector<HANDLE> print;
	vector<HANDLE> close;
	char EventID[10];
	vector<PROCESS_INFORMATION> pi;
	char key = 0;

	while (true)
	{
		if (_kbhit())
		{

			key = _getch();
			rewind(stdin);
			if (key == '+')
			{
				snprintf(EventID, 10, " %dc", close.size());
				close.push_back(CreateEvent(NULL, TRUE, FALSE, EventID));

				snprintf(EventID, 10, " %d", print.size());
				print.push_back(CreateEvent(NULL, TRUE, FALSE, EventID));

				pi.push_back(CreateNewProcess(path, EventID));
			}

			if (key == 'q')
			{
				while (pi.size()>0)
				{
					SetEvent(close.back());
					WaitForSingleObject(pi.back().hProcess, INFINITE);
					CloseHandle(close.back());
					CloseHandle(print.back());
					CloseHandle(pi.back().hProcess);
					CloseHandle(pi.back().hThread);
					close.pop_back();
					print.pop_back();
					pi.pop_back();
				}
				system("pause");
				return 0;
			}

			if (key == '-' && close.size()>0)
			{
				{
					if (print.size() == procCounter)
						procCounter--;
					SetEvent(close.back());
					WaitForSingleObject(pi.back().hProcess, INFINITE);
					CloseHandle(pi.back().hProcess);
					CloseHandle(pi.back().hThread);
					CloseHandle(close.back());
					CloseHandle(print.back());
					close.pop_back();
					print.pop_back();
					pi.pop_back();
				}
			}

			if (key == 'q')
			{
				while (pi.size() > 0)
				{
					SetEvent(close.back());
					WaitForSingleObject(pi.back().hProcess, INFINITE);
					CloseHandle(pi.back().hProcess);
					CloseHandle(pi.back().hThread);
					CloseHandle(close.back());
					CloseHandle(print.back());
					close.pop_back();
					print.pop_back();
					pi.pop_back();
				}
				return 0;
			}

		}

		if (print.size() > procCounter)
			if (procCounter == 0)
			{
				SetEvent(print[0]);
				procCounter++;
			}
			else
				if (!(WaitForSingleObject(print[procCounter - 1], 1) == WAIT_OBJECT_0))
				{
					SetEvent(print[procCounter]);
					procCounter++;
				}

		if (print.size() == procCounter && print.size() != 0)
			if (!(WaitForSingleObject(print[procCounter - 1], 1) == WAIT_OBJECT_0))
			{
				procCounter = 1;
				SetEvent(print[0]);
			}
	}
	return 0;
}