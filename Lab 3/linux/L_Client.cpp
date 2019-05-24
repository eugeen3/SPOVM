#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <wait.h>
#include <semaphore.h>
#include <string.h>
#include <string.h>

#define SEM_ID 1000
#define SHM_ID 2000

int Close = 0;

void ShowError()
{
	printf("Error in client\n\r");
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

void setClose(int sign)
{
	Close = 1;
}

int main(int argc, char *argv[])
{
	Close = 0;

	int sem_id = semget(SEM_ID, 0, 0);	//получение доступа к семафорам
	if (sem_id == -1) ShowError();

	int shm_id = shmget(SHM_ID, 40, 0);	//получение доступа к shared memory
	if (shm_id == -1) ShowError();
	void *sharedBuffer = shmat(shm_id, NULL, 0);

	struct sigaction closeSignal;
	closeSignal.sa_handler = setClose;
	sigaction(SIGUSR2, &closeSignal, NULL);

	char* buffer = argv[0];
	char empty[40] = { '\0' };
	while (!Close)
	{
		if (WaitSemaphore(sem_id, 1) != -1)
		{
			strncpy((char*)sharedBuffer, empty, 40);
			strncpy((char*)sharedBuffer, buffer, strlen(buffer));
			ReleaseSemaphore(sem_id, 0);
		}
	}
	return 0;
}