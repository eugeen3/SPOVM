#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ncurses.h"
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int Print = 0;
int Close = 0;

void setPrint(int sign)
{
    Print = 1;
    refresh();
}

void setClose(int sign)
{
    Close = 1;
    refresh();
}

int main(int argc, char* argv[])
{
    Close = 0;
    initscr();
    noecho();
    refresh();

	struct sigaction printSignal, closeSignal;

	printSignal.sa_handler = setPrint;
    sigaction(SIGUSR1,&printSignal,NULL);

    closeSignal.sa_handler = setClose;
    sigaction(SIGUSR2, &closeSignal,NULL);

    char path[8] = "./child";
/*
    char* order[12];
    order[0] = path;
    int cnt = 1;

    int choice = -1;
    while (choice != 0) {
        printw("Select genre (you can choose %d more):", 11 - cnt);
        printw("\n1.hip-hop\n2.rock\n3.electronic\n4.classical\n5.pop\n0.exit\n");
        scanw("%d", &choice);
        switch (choice) {
        case 1:
            order[cnt] = "hip-hop";
            break;
        case 2:
            order[cnt] = "rock";
            break;
        case 3:
            order[cnt] = "electronic";
            break;
        case 4:
            order[cnt] = "classical";
            break;
        case 5:
            order[cnt] = "pop";
            break;
        }
        clear();
        if (choice != 0) cnt++;
    }

    char cash[10];
    printw("Enter amount of money\n");
    scanw("%s", cash);
    order[cnt] = cash;
    order[cnt + 1] = NULL;

    order[cnt] = NULL;
    clear();
*/
    pid_t pid = fork();
    int st;

    switch(pid)
    {
        case -1: {
            printw("Error.");
            break;
        }

        case 0: {
            //if (execvp(path, order) == -1)
            if (execlp(path, path, NULL) == -1)
              printw("Error.");

            break;
        }
/*
        default: {
            while(1)
            {
              refresh();
              if(waitpid(pid,&st,WNOHANG)>0)
                  break;

              napms(50);

            }
            

            if (WIFEXITED(st))
            {
                move(21,0);
                printw("%d", (int)WEXITSTATUS(st));
            }
*/
    }

/*
    move(30,0);
    switch (st) {
    case 0:
        printw("Purchase was made succesfully!\n");
        break;
    case 1:
        printw("Not enough albums in store\n");
        break;
    case 2:
        printw("Not enough money\n");
        break;
    }
*/
    kill(getppid(), SIGUSR1);
    refresh();
    while(!Close)
    {
        printSignal.sa_handler = setPrint;
        sigaction(SIGUSR1,&printSignal,NULL);

        closeSignal.sa_handler = setClose;
        sigaction(SIGUSR2, &closeSignal,NULL);
        usleep(100);
        
        if(Print)
        {
            refresh();
            for(int i = 0; i < strlen(argv[1]); i++)
            {
                if(Close)
                {
                    Print = 0;
					kill(getppid(), SIGUSR2);
                    endwin();
                    return 0;
                }
                printw("%c",argv[1][i]);
                refresh();
                usleep(100000);
            }
            printw("%s\n\r",argv[2]);
            refresh();

            if((kill(getppid(), SIGUSR2))==0)
            Print = 0;
        }
    }

    if(waitpid(pid,&st,WNOHANG)>0) {
        clear();
	    return 0;
    }
}
