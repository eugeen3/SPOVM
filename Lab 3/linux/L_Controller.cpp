#include <vector>
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <wait.h>
#include <semaphore.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>

#define SERVER_PATH "./L_Server"
#define CLIENT_PATH "./L_Client"

using namespace std;

void ShowError()
{
	printw("Error controller\n\r");
	refresh();
}

void action_plus(vector<pid_t>& pid)
{
	char buff[255] = { '\0' };
	snprintf(buff, 255, "This process number %d\n\r", (int)pid.size());

	pid.push_back(fork());	//в массив процессов загружаем новый дочерний процесс

	switch (pid.back())
	{
	case -1:
		printw("Fork error\n\r");
		refresh();
		return;
	case 0:
		if (execl(CLIENT_PATH, buff, NULL) == -1)
		{
			printw("Client run error\n\r");
			refresh();
		}
		break;
	}
}

void action_minus(vector<pid_t>& pid)
{
	if (!pid.empty())
	{
		kill(pid.back(), SIGUSR2);	//послать последнему процессу сигнал о закрытии
		waitpid(pid.back(), 0, 0);	//подождать пока он закроется
		pid.pop_back();	//удалить из массива, т.к. процесс уже не нужен
	}
}

void action_q(vector<pid_t>& pid)
{
	while (!pid.empty())
	{
		action_minus(pid);
	}
}

int main()
{
	initscr();
	refresh();
	clear();
	noecho();

	vector<pid_t> pid;

	//run Server
	pid_t ServerPid;
	switch (ServerPid = fork())
	{
	case -1:
		ShowError();
		return 0;

	case 0:
	{
		if (execl(SERVER_PATH, SERVER_PATH, NULL) == -1)
		{
			printw("error of server creation\n\r");
			refresh();
			return 0;
		}
		break;
	}
	}

	char key;
	while (true)
	{
		halfdelay(0);
		key = (char)getchar();
		switch (key)
		{
		case '+':
			action_plus(pid);
			break;
		case '-':
			action_minus(pid);
			break;
		case 'q':
			action_q(pid);
			kill(ServerPid, SIGUSR1);
			waitpid(ServerPid, 0, 0);
			clear();
			endwin();
			return 0;
		}
	}
	return 0;
}