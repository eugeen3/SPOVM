#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ncurses.h"
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <vector>
#include <iostream>
#include <termios.h>
#include <fcntl.h>

using namespace std;

int ready = 0; 
int print = 1;

void setPrint(int sign)
{
    print = 1;
    refresh();
}

void setReady(int sign)
{
    ready = 1;
    refresh();
}

int main()
{
    initscr();
    clear();
    refresh();
    struct sigaction startPrint;
    struct sigaction freePrint;
    char string[100];
    char buff[10];
    char path[9]= "./parent";
    vector<pid_t> pid;

    startPrint.sa_handler = setPrint;
    sigaction(SIGUSR2,&startPrint,NULL);

    freePrint.sa_handler = setReady;
    sigaction(SIGUSR1,&freePrint,NULL);

    snprintf(string, 100, "Process number ");
    int currentProcess=0;
    char key;
    noecho();

    while(true)
    {
        refresh();
        startPrint.sa_handler = setPrint;
        sigaction(SIGUSR2,&startPrint,NULL);

        freePrint.sa_handler = setReady;
        sigaction(SIGUSR1,&freePrint,NULL);

        if(print && pid.size()>currentProcess)
        {
            kill(pid[currentProcess],SIGUSR1);
            currentProcess ++;
            print = 0;
            if(currentProcess == ((int)pid.size()))
            {
                currentProcess=0;
            }
        }

        halfdelay(1);
        key = getch();
        
            if (key == '+')
            {
                pid.push_back(fork());
                ready = 0;
                switch (pid.back())
                {
                    case -1:
                        cout << "Error" << endl;
                        return 0;
                    case 0:
                        snprintf(buff, 10, "%d",(int)pid.size());
                        if  (execlp(path, path, string, buff, NULL)==-1)
                        {
                            cout << "Parent process wasn't created" << endl;
                        }
                        break;
                }
                while(!ready)
                usleep(10000);
            }

            if (key == '-' && pid.size() > 0)
            {
                kill(pid.back(), SIGUSR2);
                waitpid(pid.back(),0,0);
                pid.pop_back();

                if(currentProcess == (int)pid.size())
                    currentProcess--;
            }

            if (key == 'q')
            {
                while(pid.size()>0)
                {
                    kill(pid.back(),SIGUSR2);
                    waitpid(pid.back(),0,0);
                    pid.pop_back();
                }
                clear();
                endwin();
                return 0;
            }
            key = '0';
    }

    return 0;
}
