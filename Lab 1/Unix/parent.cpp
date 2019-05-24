#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

int main()
{
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
		system("clear");
	}

	char cash[10];
	printf("Enter amount of money\n");
	scanf("%s", cash);

	char path[200] = "./child ";

	strcat(path, order);
	strcat(path, cash);

	time_t ltime;
	pid_t pid = fork();
	int st;
	
	switch(pid)
	{
		case -1: {
			printf("Error.");
			break;
		}

		case 0: {
			if(execvp(path, NULL) == -1)
				printf("Error.");

			break;
		}

		default: {
			while(1)
			{
				system("clear");
				time(&ltime);
				printf(ctime( &ltime ));

				if(waitpid(pid,&st,WNOHANG)>0)
					break;

				delay(50);
                                
			}
			if (WIFEXITED(st))
			{
				printf("%d", (int)WEXITSTATUS(st));
			}
		}
	}
	

	switch (st) {
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
	
	free(order);
	return 0;
}
