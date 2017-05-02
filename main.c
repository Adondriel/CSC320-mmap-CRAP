/*
 * Written By:
 * Courtney Rush
 * Adam Pine
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <memory.h>

#define bufsize 500
#define delim " "

char **tokenizeLine(char *line) {
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token, **tokens_backup;
    int pos = 0;
    int newbuf = 0;
    token = strtok(line, delim);
    /* walk through other tokens */
    while (token != NULL) {
        newbuf += bufsize;
        tokens[pos] = token;
        pos++;
        if (pos >= bufsize) {
            tokens_backup = tokens;
            tokens = realloc(tokens, newbuf * sizeof(char *));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        //printf("%s ", token);
        token = strtok(NULL, delim);
    }
    return tokens;
}

int main(int argc, char *argv[]) {

    if (argc == 2) {
        //One command line argument
        int pid = atoi(argv[1]);
        //printf("%d", pid);
        pid_t return_pid = kill(pid, 0);
        if (return_pid != -1) {
            //setup the file locations
            char mapsStr[25], pagemapStr[25];
            sprintf(mapsStr, "/proc/%d/maps", pid);
            sprintf(pagemapStr, "/proc/%d/pagemap", pid);

            //setup tokens for later tokenization.
            char **tokens = malloc(bufsize * sizeof(char *));
            int pos = 0;
            int newbuf = 0;
            //check location
            if (access(mapsStr, F_OK) != -1 && access(pagemapStr, F_OK) != -1) {
                printf("maps and pagemap exists\n");
                FILE *fp = fopen(mapsStr, "r");
                char line[256];
                char lastSegment[256];
                //assemble array of lines.
                while (fgets(line, sizeof(line), fp)) {
                    tokens = tokenizeLine(line);
                    if (atoi(tokens[4]) > 0) {
                        char newSegment[256];
                        strcpy(newSegment, tokens[5]);
                        if (strcmp(newSegment, lastSegment)) {
                            printf("\n----------------------------------------------------\n");
                            strcpy(lastSegment, newSegment);
                        }
                        for(int i=0; i <= 5; i++){
                            printf("%s ", tokens[i]);
                        }
                        //printf("\nnth index: %s\n", tokens[5]);
                    }


                }
                fclose(fp);
            } else {
                printf("maps or pagemap file does not exist");
            }
        } else {
            /* error */
            printf("Pid not found");
        }
    } else {
        printf("Expected a total of 1 argument.");
    }
    return 0;
}

//ugly WIP comments
//for(int i = 0; i < pos-1; i++){
//printf("%s\n", tokensArray[i][0]);
//}

/*
 *                     newbuf += bufsize;
    tokensArray[pos] = tokenizeLine(line);
    pos++;
    if (pos >= bufsize){
        tokensArray = realloc(tokensArray, newbuf * sizeof(char**));
        if (!tokensArray){
            printf("allocation error");
            exit(EXIT_FAILURE);
        }
    }
 *
 * get the first token

} else {
    printf("\nNULL PNTR\n");
}                    */