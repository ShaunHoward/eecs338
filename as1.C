#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static char WHALE[] = "WHALE";
char P0[] = "PO";
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
    char hostname[256];

    //print process id, parent process id, hostname, username, time of day, cwd, that it is the parent process.
    fflush(stdout);
    if(gethostname(hostname, sizeof(hostname)) != 0){
       perror("gethostname");
    }

    char *login_name = getlogin();

    if (login_name == NULL){
        perror("cuserid");
    }

    time_t curr_time;
    char* time_str = ctime(&curr_time);
    int status = printf("pid: %d\nppid: %d\nhostname: %s\nusername: %s\ntime: %s\ncwd: %s\n This is the parent process.",
                     getpid(), getppid(), hostname, login_name, time_str, getcwd(NULL, 0));
    if (status != 0){
        perror("Error printing pid, ppid, hostname, username, time, cwd, etc.");
        fflush(stderr);
    }
    fflush(stdout);
}

int print_child_process_info(char *process){
    int status = printf("Child process %s that has pid: %d, and parent pid: %d\n",
                    process,
                    getpid(),
                    getppid());
    fflush(stdout);
    return status;
}

int print_shrimp_line(char *process){
    char *whale = getenv("WHALE");
    int status = printf("%s: %s shrimp (WHALE environment variable value is now %s)\n",
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
    //cheap conversion from int to string
    var = '0' + val;
    int status = setenv(env_var, var, 1);
    if (status != 0){
        perror("Error setting environment variable WHALE.");
    }
    return status;
}

int tock_and_subtract(int wait_time, int subtr, char process[]){
    int status = 0;
    tock(wait_time);
    status += subtract_from_var(WHALE, subtr);
    status += print_shrimp_line(process);
    return status;
}

int run_parent_process(){
    int status = 0;
    status += tock_and_subtract(3, 1, P0);
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
    status += printf("WHALE is now %s", getenv(WHALE));
    return status;
}

int await_return(char process[]){
    int return_;
    pid_t proc = wait(&return_);
    int status = return_;
    status += printf("%s terminated with pid: %d\n", process, proc);
    fflush(stdout);
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
    status += printf("Directory changed to root (\\)\n");
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
    status += printf("Current working directory: %s\n", cwd);
    fflush(stdout);
    _exit(0);

    return status;
}

int run_child_processes(int count){
    int status = 0;
    for(int i = 0; i < count; i++){
        pid_t pid = fork();
        status += printf("Child process spawned with pid: %d\n", pid);
        fflush(stdout);
    	if (pid == -1) {
    	    //fork error
            perror("Error in forking.")
    	} else if (pid == 0){
    	   //I am the child, my pid is getpid()
    	   printf("Child process running. Pid is: %d", getpid());
    	   printf("Parent of current process pid is: %d", getppid());
    	   fflush(stdout);
    	   if (i == 0) {
        		if (status == 0 && i == 0){
                    status += run_C1();
                } else if (status == 0 && i == 1){
                    status += run_C2();
                } else {
                    perror(ERR_MSG);
					fflush(stderr);
                    return status;
                }
    		}
    	} else {
    	   sleep(1);
            //I am the parent, my childs pid is pid
    	   printf("Parent process running. Child pid is: ", pid);
    	   fflush(stdout);
    	}
    }
    return status;
}

int main(int argc, const char* argv[]){
    // track the success/failure of operations w/ running total
    int success = 0;
    success += print_base_process_info();

    //create environment variable WHALE and initialize to 7
    char whale[] = "WHALE=7";
    success += putenv(whale);

    // run 2 child processes for shrimp lines
    success += run_child_processes(2);

    // run parent process for shrimp lines
    success += run_parent_process();

    return success;
}
