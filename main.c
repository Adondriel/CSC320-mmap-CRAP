#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if(argc > 2)
    {
        printf("Too many arguments");
    }
    else if(argc < 2)
    {
        printf("One argument expected");
    }
    else
    {
        //One command line argument
        int pid = atoi(argv[1]);
        printf("%d", pid);
        pid_t return_pid = kill(pid, 0);
        if(return_pid == -1)
        {
            /* error */
            printf("Nope %d", return_pid);
        }
        else if(return_pid == 0)
        {
            /* process still running */
            printf("Yep! %d", return_pid);
        }
    }
    return 0;
}
