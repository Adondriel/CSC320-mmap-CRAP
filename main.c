/*
 * Written By:
 * Courtney Rush
 * Adam Pine
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

struct map_item {
    char *startAddr;
    char *endAddr;
    char *protection;
    char *offset;
    char *major;
    char *minor;
    char *inode;
    char *program;
};

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
        //printf("%d", pid);
        pid_t return_pid = kill(pid, 0);
        if(return_pid != -1)
        {
            /* process still running */
            //printf("Process Found %d", return_pid);
            char mapsStr[100];
            sprintf(mapsStr, "/proc/%d/maps", pid);
            char pagemapStr[100];
            sprintf(pagemapStr, "/proc/%d/pagemap", pid);
            if( access(mapsStr, F_OK ) != -1 && access(pagemapStr, F_OK ) != -1 ) {
                printf("maps and pagemap exists\n");
                FILE *fp = fopen(mapsStr, "r");
                char line[256];

                //fp = fopen(mapsStr,"r");
                while(fgets(line, sizeof(line), fp))
                {
                    printf("%s", line);
                }
                fclose(fp);

            } else {
                printf("maps or pagemage file does not exist");
            }
        }
        else
        {
            /* error */
            printf("Pid not found");
        }
    }
    return 0;
}
