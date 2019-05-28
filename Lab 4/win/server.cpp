#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <ctime>
#define PIPES
using namespace std;

void PrintLastError()
{
	printf("Create Process failed (%d)\n", GetLastError());
	_getch();
	return;
}

int main(int argc, char* argv[])
{
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, "CloseServer");
	HANDLE print = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Print");
	HANDLE conn;
	HANDLE mas[2] = { print, close };

	char buf[40];
	memset(buf, '\0', 40);

	conn = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"),
		PIPE_ACCESS_INBOUND,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		40,
		40,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	while (true)
	{
		if (ConnectNamedPipe(conn, NULL) == FALSE)
		{
			cout << "Error connecting";
		}

		if (WaitForSingleObject(close, 0) == WAIT_OBJECT_0 ||
			(WaitForMultipleObjects(2, mas, FALSE, INFINITE) == WAIT_OBJECT_0 + 1))
		{
			DisconnectNamedPipe(conn);
			CloseHandle(print);
			CloseHandle(close);
			CloseHandle(conn);
			return 0;
		}
		else
		{
			memset(buf, '\0', 40);
			ReadFile(conn, buf, 40, NULL, NULL);
			for (int i = 0; i < strlen(buf); i++)
			{
				printf("%c", buf[i]);
				Sleep(100);
			}
			DisconnectNamedPipe(conn);
		}
	}
	return 0;
}