#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// string constants for printing various Linux-related properties
static char PPROC_S[] = "This process is the parent process.";
static char PPROC[] = "The parent process id is: ";
static char PPPROC[] = "The parent of the parent process id is: ";
static char HOST[] = "The hostname is: ";
static char USER[] = "The username is: ";
static char TIME[] = "The time of day is: ";
static char WORK_DIR[] = "The working directory is: ";

static char C1_S[] = "The first child process id is: ";
static char C2_S[] = "The second child process id is: ";

static char C1_C[] = "C1: ";
static char C2_C[] = "C2: ";
static char P0_C[] = "PO: ";

// used to find the process and parent process ids
pid_t getpid(void);
pid_t getppid(void);

// define get hostname function
int gethostname(char *name, size_t len);

int main(){
    // track the success/failure of operations w/ running total
    int success = 0;
    
    // temporary variables for printing values
    char concat_temp[20];
    char val[20];

    //print process id
    sprintf(val, "%d", getpid());
    strcpy(concat_temp, PPROC);
    strcat(concat_temp, val);
    success += printf("%s\n", concat_temp);

    //print parent process id
    sprintf(val, "%d", getppid());
    strcpy(concat_temp, PPPROC);
    strcat(concat_temp, val);
    success += printf("%s\n", concat_temp);

    //print hostname
    strcpy(concat_temp, HOST);
    gethostname(val, 20);
    strcat(concat_temp, val);
    success += printf("%s\n", concat_temp);

    //print username
    strcpy(concat_temp, USER);
    strcat(concat_temp, cuserid(NULL));
    success += printf("%s\n", concat_temp);

    //print time of day
    strcpy(concat_temp, TIME);
    time_t curtime;
    time(&curtime);
    strcat(concat_temp, ctime(&curtime));
    success += printf("%s\n", concat_temp);

    //print working dir
    strcpy(concat_temp, WORK_DIR);
    strcat(concat_temp, getcwd(NULL, 0));
    success += printf("%s\n", concat_temp);

    //print that it is the parent process
    success += printf("%s\n", PPROC_S);

    //create environment variable WHALE and initialize to 7
    strcpy(val, "WHALE=7");
    success += putenv(val);

    if (success == 0) {}
        //create fork 1
        C1 = fork();

        //create fork 2
        C2 = fork();
    else {
        return success;
    }

    if (C1 == 0) {
        //print C1 pid
        sprintf(val, "%d", C1);
        strcpy(concat_temp, C1);
        strcat(concat_temp, val);
        success += printf("%s\n", concat_temp);

        //print parent process id
        sprintf(val, "%d", getppid());
        strcpy(concat_temp, PPPROC);
        strcat(concat_temp, val);
        success += printf("%s\n", concat_temp);
    } else {
        sprintf(val, "%d", C1);
        strcpy(concat_temp, PPROC);
        strcat(concat_temp, val);
        success += printf("%s\n", concat_temp);
    }

    if (C2 == 0) {
        //print C2 pid
        sprintf(val, "%d", C2);
        strcpy(concat_temp, C2);
        strcat(concat_temp, val);
        success += printf("%s\n", concat_temp);

        //print parent process id
        sprintf(val, "%d", getppid());
        strcpy(concat_temp, PPPROC);
        strcat(concat_temp, val);
        success += printf("%s\n", concat_temp);
    } else {
        sprintf(val, "%d", C2);
        strcpy(concat_temp, PPROC);
        strcat(concat_temp, val);
        success += printf("%s\n", concat_temp);
    }

    //7

    return 0;
}