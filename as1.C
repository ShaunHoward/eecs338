#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
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

void tock(int sleep_time){
    usleep(4000 * sleep_time);
}

void print_base_process_info(){
    // temporary variables for printing values
    char concat_temp[20];
    char hostname[256];

    //print process id, parent process id, hostname, username, time of day, cwd, that it is the parent process.
    fflush(stdout);
    if(gethostname(hostname, sizeof(hostname)) != 0){
       perror("gethostname");
       fflush(stderr);
    }

    char *login_name = getlogin();

    if (login_name == NULL){
        perror("cuserid");
        fflush(stderr);
    }

    char *cwd = getcwd(NULL, 0);

    if (cwd == NULL){
    	perror("getcwd");
    	fflush(stderr);
    }

    time_t curr_time;
    char* time_str = ctime(&curr_time);
    printf("pid: %d\nppid: %d\nhostname: %s\nusername: %s\ntime: %scwd: %s\nThe parent process for setting WHALE is %d\n",
           getpid(), getppid(), hostname, login_name, time_str, cwd, getpid());
    fflush(stdout);
}

void print_child_process_info(char *process){
    printf("Child process %s with pid: %d, and parent pid: %d\n",
           process,
           getpid(),
           getppid());
    fflush(stdout);
}

void print_shrimp_line(char *process){
    char *whale = getenv("WHALE");
    if (whale==NULL){
    	perror("getenv");
    	fflush(stderr);
    } else {
		printf("%s: %s shrimp (WHALE environment variable value is now %s)\n",
			   process,
			   whale,
			   whale);
		fflush(stdout);
    }
}

void subtract_from_var(char *env_var, int decr){
    char *var = getenv(env_var);
    int val = atoi(var);
    val -= decr;
    sprintf(var, "%d", val);
    int status = setenv(env_var, var, 1);
    if (status != 0){
        perror("Error setting environment variable WHALE.\n");
        fflush(stderr);
    }
}

void tock_and_subtract(int wait_time, int subtr, char process[]){
    int status = 0;
    tock(wait_time);
    subtract_from_var(WHALE, subtr);
    print_shrimp_line(process);
}

void run_parent_process(){
    tock(3);
    print_shrimp_line(P0);
    tock_and_subtract(3, 3, P0);
    tock_and_subtract(3, 3, P0);
	tock(3);
	int return_;
	pid_t proc = wait(&return_);
	printf("C1 terminated with pid: %d\n", proc);
	fflush(stdout);
	proc = wait(&return_);
	printf("C2 terminated with pid: %d\n", proc);
	fflush(stdout);

	subtract_from_var(WHALE, 1);
	char *whale = getenv(WHALE);
	if (whale == NULL){
		perror("getenv");
		fflush(stderr);
	} else {
		printf("WHALE is at final value %s\n", whale);
		fflush(stdout);
	}
}

void run_C1(){
    // start shrimp line operation
    tock(1);
    print_child_process_info(C1);
    tock_and_subtract(3, 1, C1);
    tock_and_subtract(3, 3, C1);

	tock(3);
	printf("C1: Changed current directory to root (\\)\n");
	fflush(stdout);
	chdir("/");
	execl("/bin/ls", "ls", "-la", (char*)NULL);
	_exit(errno);
}

void run_C2(){
    // start shrimp line operation
    tock(2);
    print_child_process_info(C2);

    tock_and_subtract(3, 2, C2);
    tock_and_subtract(3, 3, C2);
	tock(3);
	char cwd[MAXPATHLEN];
	if (getcwd(cwd, MAXPATHLEN) == NULL){
		perror("getcwd");
		fflush(stderr);
		_exit(errno);
	} else {
		printf("C2: Current working directory is: %s\n", cwd);
		fflush(stdout);
		_exit(0);
	}
}

int main(int argc, const char* argv[]){
    print_base_process_info();

	//create environment variable WHALE and initialize to 7
	char whale[] = "WHALE=7";
	if (putenv(whale) != 0) {
		perror("putenv");
		fflush(stderr);
		return errno;
	}

	pid_t pid = fork();
	if (pid == 0) {
		run_C1();
	} else if (pid < 0) {
		perror("C1 fork error");
	}

	pid = fork();
	if (pid == 0) {
		run_C2();
	} else if (pid < 0) {
		perror("C2 fork error");
	}

	// run parent process for shrimp lines
	run_parent_process();
    return 0;
}
