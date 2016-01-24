#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static char WHALE[] = "WHALE";
char PO[] = "PO";
char C1[] = "C1";
char C2[] = "C2";

static char ERR_MSG[] = "An error has occurred in the process.\n";

// used to find the process and parent process ids
pid_t getpid(void);
pid_t getppid(void);

// define get hostname function
int gethostname(char *name, size_t len);

int tock(int sleep_time){
    sleep((float)(sleep_time*4000) / 1000000);
}

int print_base_process_info(){
    // temporary variables for printing values
    char concat_temp[20];
    char val[256];
    int status = 0;

    //print process id
    status += fprintf(stdout, "pid: %d\n", getpid());

    //print parent process id
    status += fprintf(stdout, "parent pid: %d\n", getppid());

    //print hostname
    gethostname(val, 256);
    status += fprintf(stdout, "hostname: %s\n", val);

    //print username
    status += fprintf(stdout, "username: %s\n", cuserid(NULL));

    //print time of day
    time_t curtime;
    time(&curtime);

    status += fprintf(stdout, "current time: %s\n", ctime(&curtime));

    //print working dir
    status += fprintf(stdout, "current working directory: %s\n", getcwd(NULL, 0));

    //print that it is the parent process
    status += fprintf(stdout, "This process is the parent process.\n");

    return 0;
}

int print_child_process_info(char *process){
    int status = fprintf(stdout, "Child process %s that has pid: %d, and parent pid: %d\n",
                    process,
                    getpid(),
                    getppid());
    return status;
}

int print_shrimp_line(char *process){
    char *whale = getenv("WHALE");
    int status = fprintf(stdout, "%s: %s shrimp (WHALE environment variable value is now %s)\n",
                    process,
                    whale,
                    whale);
    fflush(stdout);
    return status;
}

int subtract_from_var(char *env_var, int decr){
    char *var = getenv(env_var);
    int val = atoi(var);
    val -= decr;
    itoa(val, var, 10);
    int status = setenv(env_var, var, 1);
    return status;
}

int tock_and_subtract(int wait_time, int subtr, char[] process){
    int status = 0;
    tock(wait_time);
    status += subtract_from_var(WHALE, subtr);
    status += print_shrimp_line(process);
    return status;
}

int run_parent_process(){
    int status = 0;
    status += tock_and_subtract(3, 1, PO);
    status += tock_and_subtract(3, 3, P0);
    status += tock_and_subtract(3, 3, P0);
    status += tock_and_subtract(3, 3, P0);

    tock(3);
    int return_;
    pid_t proc = wait(&return_);
    fprintf(stdout, "C0 terminated with pid: %d\n", proc);
    status += await_return(C1);
    status += await_return(C2);
    status += subtract_from_var(WHALE, -1);
    status += fprintf(stdout, "WHALE is now %s", getenv(WHALE));
    return status;
}

int await_return(char[] process){
    int return_;
    pid_t proc = wait(&return_);
    int status = return_;
    status += fprintf(stdout, "%s terminated with pid: %d\n", process, proc);
    return status;
}

int run_C1(){
    int status = 0;

    // start shrimp line operation
    tock(1);
    status += print_child_process_info(C1);

    status += tock_and_subtract(3, 2, C1);
    status += tock_and_subtract(3, 3, C1);
    status += tock_and_subtract(3, 3, C1);

    tock(3);
    status += fprintf(stdout, "Directory changed to root (\\)\n");
    fflush(stdout);
    chdir("/");
    execl("/bin/ls", "ls -la", (char*)NULL);
    _exit(0);

    return status;
}

int run_C2(){
    int status = 0;

    // start shrimp line operation
    tock(2);
    status += print_child_process_info(C2);

    status += tock_and_subtract(3, 3, C2);
    status += tock_and_subtract(3, 3, C2);
    status += tock_and_subtract(3, 3, C2);

    tock(3);
    char cwd[MAXPATHLEN];
    getcwd(cwd, MAXPATHLEN);
    status += fprintf(stdout, "Current working directory: %s\n", cwd);
    fflush(stdout);
    _exit(0);

    return status;
}

int run_child_processes(int count){
    int status = 0;
    for(int i = 0; i < count; i++){
        pid_t fork_name = fork();
        status += fork_name;
        status += fprintf(stdout, "Child process spawned with pid: %d\n", fork_name);
        fflush(stdout);
        if (status == 0 && i == 0){
            status += run_C1();
        } else if (status == 0 && i == 1){
            status += run_C2();
        } else {
            status += fprintf(stdout, "%s", ERR_MSG);
            return status;
        }
    }
}

int main(int argc, const char* argv[]){
    // track the success/failure of operations w/ running total
    int success = 0;
    success += print_base_process_info();

    //create environment variable WHALE and initialize to 7
    static char *WHALE_ = "WHALE=7";
    success += putenv(WHALE_);

    // run 2 child processes for shrimp lines
    success += run_child_processes(2);
    
    // run parent process for shrimp lines
    success += run_parent_process();

    return success;
}