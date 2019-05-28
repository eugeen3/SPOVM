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
#include <ncurses.h>
#include <pthread.h>

#define SEM_KEY 1000
#define SHM_KEY 2000
#define SERVER_PATH "./server"

using namespace std;

pthread_mutex_t mutex;

void ShowError()
{
	printw("Error controller\n\r");
	refresh();
}

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
} semunion;

int WaitSemaphore(int sem_id, int num)
{
	struct sembuf buf;
	buf.sem_op = -1;
	buf.sem_flg = 0;
	buf.sem_num = num;
	return semop(sem_id, &buf, 1);
}

int ReleaseSemaphore(int sem_id, int num)
{
	struct sembuf buf;
	buf.sem_op = 1;
	buf.sem_flg = IPC_NOWAIT;
	buf.sem_num = num;
	return semop(sem_id, &buf, 1);
}

void* myThread(void* data)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	int sem_id = semget(SEM_KEY, 0, 0);	//��������� ������� � ���������
	if (sem_id == -1) ShowError();

	int shm_id = shmget(SHM_KEY, 40, 0);	//��������� ������� � shared memory
	if (shm_id == -1) ShowError();
	void *sharedBuffer = shmat(shm_id, NULL, 0);

	char buffer[40];
	strncpy(buffer, (char*)data, 40);
	char empty[40] = { '\0' };

	while (1)
	{
		if (WaitSemaphore(sem_id, 1) != -1)
		{
			pthread_mutex_lock(&mutex);
			strncpy((char*)sharedBuffer, empty, 40);
			strncpy((char*)sharedBuffer, buffer, strlen(buffer));
			ReleaseSemaphore(sem_id, 0);
			pthread_mutex_unlock(&mutex);
			pthread_testcancel();
		}
	}
}

void action_plus(vector<pthread_t>& pid)
{
	char buff[255] = { '\0' };
	snprintf(buff, 255, "Thread number %d\n\r", (int)pid.size());

	pthread_t newThread;
	pthread_create(&newThread, NULL, myThread, buff);
	pid.push_back(newThread);
}

void action_minus(vector<pthread_t>& pid)
{
	if (!pid.empty())
	{
		pthread_cancel(pid.back());
		pthread_join(pid.back(), NULL);		
		pid.pop_back();	
	}
}

void action_q(vector<pthread_t>& pid)
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

	vector<pthread_t> pid;

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

