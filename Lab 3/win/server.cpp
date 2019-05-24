#define PIPES
#include <conio.h>
#include <Windows.h>
#include <iostream>
using namespace std;

/*
struct genre {
	char name[20];
	int amt_in_store = 0;
	int amt_in_order = 0;
	int price = 0;
};

void read_from_file(genre*);
void rewrite_file(genre*);
*/

int main(int argc, char *argv[]) {

	char order[40];

	HANDLE print = CreateSemaphore(NULL, 0, 1, "Print");
	HANDLE isFree = CreateSemaphore(NULL, 1, 1, "IsFree"); 
	HANDLE close = OpenEvent(EVENT_ALL_ACCESS, FALSE, "CloseServer");

	HANDLE conn = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"),
		PIPE_ACCESS_INBOUND,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		40,
		40,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	HANDLE massiv[2] = { print, close };

	while (true)
	{
		if (WaitForSingleObject(close, 0) == WAIT_OBJECT_0)
		{
			CloseHandle(print);
			CloseHandle(isFree);
			CloseHandle(close);
			CloseHandle(conn);
			return 0;
		}

		if (ConnectNamedPipe(conn, NULL) == FALSE)
		{
			cout << "Error connecting";
		}
	
		if (WaitForMultipleObjects(2, massiv, FALSE, INFINITE) == WAIT_OBJECT_0 + 1)
		{
			//DisconnectNamedPipe(conn);
			CloseHandle(print);
			CloseHandle(isFree);
			CloseHandle(close);
			CloseHandle(conn);
			return 0;
		}
		else
		{
			memset(order, '\0', 40);
			ReadFile(conn, order, 40, NULL, NULL);
			for (int i = 0; i < strlen(order); i++)
			{
				printf("%c", order[i]);
				Sleep(70);
			}
			DisconnectNamedPipe(conn);
			ReleaseSemaphore(isFree, 1, NULL);
		}
	}

/*
	genre order[5];

	read_from_file(order);

	int cash = atoi(argv[argc - 1]);

	getchar();
	int j = 1, i = 0;
	while (j < argc - 1) {
		if (i == 5) i = 0;
		if (!strcmp(order[i].name, argv[j])) {
			if (order[i].amt_in_store == 0 || order[i].amt_in_store == order[i].amt_in_order)
				return 1;
			else order[i].amt_in_order++;

			if (order[i].price > cash) return 2;
			else cash -= order[i].price;

			j++;
		}
		else  i++;
	}

	rewrite_file(order);

	return 0;
	*/
}
/*
void read_from_file(genre *array) {
	FILE *fptr;
	if ((fptr = fopen("store.txt", "rt")) == NULL) {
		printf("Cannot open file");
		exit(0);
	}

	for (int i = 0; i < 5; i++) {
		int current = 0;
		while (array[i].name[current - 1] != ' ') {
			fscanf(fptr, "%c", &array[i].name[current]);
			current++;
		};
		array[i].name[current - 1] = '\0';

		fscanf(fptr, "%d", &array[i].amt_in_store);
		fscanf(fptr, "%d", &array[i].price);
		fgetc(fptr);
	}

	fclose(fptr);
}

void rewrite_file(genre  *array) {
	FILE *fptr;
	if ((fptr = fopen("store.txt", "wt")) == NULL) {
		printf("Cannot open file");
		exit(0);
	}

	for (int i = 0; i < 5; i++) {
		fprintf(fptr, "%s ", array[i].name);
		fprintf(fptr, "%d ", array[i].amt_in_store - array[i].amt_in_order);
		fprintf(fptr, "%d\n", array[i].price);
	}
	fclose(fptr);
}
*/