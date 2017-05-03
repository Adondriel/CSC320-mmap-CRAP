/*
 * Written By:
 * Courtney Rush
 * Adam Pine
 */

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <fcntl.h>

#define bufsize 500
#define delim " "
#define PFN_MASK ((1ULL << 55) - 1)
#define PRESENT_MASK (1ULL << 63)
#define SWAPPED_MASK (1ULL << 62)
#define SWAPTYPE_MASK ((1ULL << 6)-1)


typedef struct foo {
    unsigned x:1;
} foo;

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

char **tokenizeAddr(char *line) {
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token, **tokens_backup;
    int pos = 0;
    int newbuf = 0;
    token = strtok(line, "-");
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
        token = strtok(NULL, "-");
    }
    return tokens;
}

unsigned long getPageNumber(unsigned long addr) {
    unsigned long pagesize = getpagesize();
    return (addr / pagesize);
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
            int pos = 0;
            int newbuf = 0;
            //check location
            if (access(mapsStr, F_OK) != -1 && access(pagemapStr, F_OK) != -1) {
                char **tokens = malloc(bufsize * sizeof(char *));
                printf("maps and pagemap exists\n");
                FILE *fp = fopen(mapsStr, "r");
                char line[256];
                char lastSegment[256];
                //assemble array of lines.
                FILE *pmFP = fopen(pagemapStr, "r");
                int present =0;
                int pages =0;
                int swapped =0;
                int notpresent=0;
                while (fgets(line, sizeof(line), fp)) {
                    tokens = tokenizeLine(line);
                    char newSegment[256];
                    strcpy(newSegment, tokens[5]);
                    char **addrTokens = tokenizeAddr(tokens[0]);
                    //printf("%s %s\n", addrTokens[0], addrTokens[1]);
                    uint64_t buf;
                    //printf("strt addr hex: %s\n", addrTokens[0]);
                    unsigned long startAddrL = strtol(addrTokens[0], NULL, 16);
                    unsigned long endAddrL = strtol(addrTokens[1], NULL, 16);
                    //printf("strt addr int: %"PRIx64, startAddrL);
                    //printf("\nend addr int: %"PRIx64, endAddrL);

                    if (strcmp(newSegment, lastSegment)) {
                        printf("\n\nSegment: %s", newSegment);
                        printf("%s (%s-%s)", tokens[1], addrTokens[0], addrTokens[1]);
                        printf("\n----------------------------------------------------\n");
                        strcpy(lastSegment, newSegment);
                    }
                    fseek(pmFP, (sizeof(uint64_t) * getPageNumber(startAddrL)), SEEK_SET);
                    fread(&buf, sizeof(uint64_t), 1, pmFP);
                    if ((buf & PRESENT_MASK)==0){
                        (void)printf("%s (%#lx): [ram](shift=0, swapped=%llu, present=0, pfn=%llu\n", addrTokens[0], startAddrL, buf&SWAPPED_MASK, buf & PFN_MASK);
                        notpresent++;
                    } else
                    {
                        if ((buf& SWAPPED_MASK)==0){
                            (void)printf("%s (%#lx): [ram](shift=12, swapped=%llu, present=1, pfn=%llu\n", addrTokens[0], startAddrL, buf&SWAPPED_MASK, buf & PFN_MASK);
                        }else{
                            (void)printf("%s (%#lx): [swap](type=%llu, offset=%llu)\n", addrTokens[0], startAddrL, buf&SWAPTYPE_MASK, (buf & PFN_MASK)-(buf&SWAPTYPE_MASK));
                            swapped++;
                        }
                        present++;
                    }
                    pages++;
                }
                printf("Total Pages: %d (%.1fMB)\n", pages, ((double)pages*getpagesize())/1024/1024);
                printf("Total Present: %d (%.1fMB)\n", present, ((double)present*getpagesize())/1024/1024);
                printf("Total Swapped: %d (%.1fMB)\n", swapped, ((double)swapped*getpagesize())/1024/1024);
                printf("Total Not Present: %d (%.1fMB)\n", notpresent, ((double)notpresent*getpagesize())/1024/1024);

                fclose(fp);
                fclose(pmFP);
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