#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <termios.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <wait.h>
#include <semaphore.h>
#include <string.h>
#include <iostream>

#define SEM_ID 1000
#define SHM_ID 2000

using namespace std;

void ShowError()
{
	printw("Error server\n\r");
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

int Close = 0;

void setClose(int sign)
{
	Close = 1;
}

int main(int argc, char* argv[])
{
	int sem_id = semget(SEM_ID, 2, IPC_CREAT | 0777);	//2 семафора: 0-ой - на факт записи в память, 1-ый - на факт освобождения памяти
	if (sem_id == -1) ShowError();
	
	semctl(sem_id, 0, SETVAL, 0);	//initialization
	semctl(sem_id, 1, SETVAL, 1);	//initialization


	int shm_id = shmget(SHM_ID, 40, IPC_CREAT | 0777);	//разделяемая память	
	if (shm_id == -1) ShowError();

	char buffer[40];
	void *sharedBuffer = shmat(shm_id, NULL, 0);

	struct sigaction closeSignal;
	closeSignal.sa_handler = setClose;
	sigaction(SIGUSR1, &closeSignal, NULL);

	initscr();
	clear();
	refresh();

	while (!Close)
	{
		if (WaitSemaphore(sem_id, 0) != -1)
		{
			memset(buffer, '\0', 40);
			strncpy(buffer, (char*)sharedBuffer, 40);
			for (int i = 0; i <= strlen(buffer); i++)
			{				
				printw("%c", buffer[i]);
				refresh();
				usleep(100000);
			}
			ReleaseSemaphore(sem_id, 1);
		}
	}
	semctl(sem_id, 0, IPC_RMID, semunion);
	semctl(sem_id, 1, IPC_RMID, semunion);
	shmdt(sharedBuffer);
	shmctl(shm_id, IPC_RMID, NULL);
	clear();
	endwin();
	return 0;
}