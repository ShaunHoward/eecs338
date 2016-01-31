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

// used to find the process and parent process ids
pid_t getpid(void);
pid_t getppid(void);

// used to find the hostname of the process
int gethostname(char *name, size_t len);

/**
 * Sleep for the given amount of time.
 */
void tock(int sleep_time){
    usleep(4000 * sleep_time);
}

/**
 * Prints the current process id, its parent process id, the hostname, username, time of day,
 * current working directory, and that it is the parent process. All sys calls are checked
 * for errors and output streams are flushed after print.
 */
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

    // try to get the login name
    char *login_name = getlogin();

    // print error if cannot get login name
    if (login_name == NULL){
        perror("cuserid");
        fflush(stderr);
    }

    // try to get the cwd
    char *cwd = getcwd(NULL, 0);

    // print error if cannot get cwd
    if (cwd == NULL){
    	perror("getcwd");
    	fflush(stderr);
    }

    // get the current time in a pretty-printed format
    time_t curr_time;
    char* time_str = ctime(&curr_time);

    // print all information
    printf("pid: %d\nppid: %d\nhostname: %s\nusername: %s\ntime: %scwd: %s\nThe parent process for setting WHALE is %d\n",
           getpid(), getppid(), hostname, login_name, time_str, cwd, getpid());
    fflush(stdout);
}

/**
 * Prints information about the child process id and its parent pid.
 */
void print_child_process_info(char *process){
    printf("Child process %s with pid: %d, and parent pid: %d\n",
           process,
           getpid(),
           getppid());
    fflush(stdout);
}

/**
 * Prints the executing process, the shrimp line, and the value of WHALE.
 */
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

/**
 * Subtracts the given decrement from the given environment variable
 * (assuming it has an integer type) and resets the value to the
 * decremented value.
 */
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

/**
 * Sleeps for the given number of time steps, then subtracts
 * the given value from the environment variable WHALE,
 * then prints an updated shrimp line about the new value of WHALE.
 */
void tock_and_subtract(int wait_time, int subtr, char process[]){
    int status = 0;
    tock(wait_time);
    subtract_from_var(WHALE, subtr);
    print_shrimp_line(process);
}

/**
 * Executes the P0 parent process.
 * Prints the first line with initial value of WHALE at 7.
 * Then, sleeps and subtracts from WHALE twice.
 * Waits until children C1 and C2 terminated, then retrieves
 * WHALE, decrements it to 0, prints the value, and exits.
 * The user will be notified of errors.
 */
void run_parent_process(){
    tock(3);
    print_shrimp_line(P0);

    // sleep and subtract from WHALE twice
    tock_and_subtract(3, 3, P0);
    tock_and_subtract(3, 3, P0);
	tock(3);
	int return_;

	// wait for children C1 and C2 to return
	pid_t proc = wait(&return_);
	printf("C1 terminated with pid: %d\n", proc);
	fflush(stdout);
	proc = wait(&return_);
	printf("C2 terminated with pid: %d\n", proc);
	fflush(stdout);

	// subtract from WHALE last time
	subtract_from_var(WHALE, 1);
	char *whale = getenv(WHALE);

	// try to get WHALE if possible and print its value
	if (whale == NULL){
		perror("getenv");
		fflush(stderr);
	} else {
		printf("WHALE is at final value %s\n", whale);
		fflush(stdout);
	}
}

/**
 * Executes the C1 child process.
 * Prints process info, sleeps and subtracts from WHALE twice.
 * Then, prints the current working directory and exists.
 */
void run_C1(){
    // start shrimp line operation
    tock(1);
    print_child_process_info(C1);

    // sleep and subtract from WHALE twice
    tock_and_subtract(3, 1, C1);
    tock_and_subtract(3, 3, C1);

	tock(3);

	// change the cwd to root if possible, then run ls -la command
	// exit if any errors occur in sys calls
	if (chdir("/") < 0) {
		perror("chdir");
		_exit(errno);
	} else {
		printf("C1: Changed current directory to root (\\)\n");
		fflush(stdout);
		execl("/bin/ls", "ls", "-la", (char*)NULL);
		_exit(errno);
	}
}

/**
 * Executes the C2 child process.
 * Prints process info, sleeps and subtracts from WHALE twice.
 * Then, prints the current working directory and exists.
 */
void run_C2(){
    // start shrimp line operation
    tock(2);
    print_child_process_info(C2);

    // sleep and subtract from WHALE twice
    tock_and_subtract(3, 2, C2);
    tock_and_subtract(3, 3, C2);
	tock(3);
	char cwd[MAXPATHLEN];

	// get and print the cwd if possible
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
	// print info about console session
    print_base_process_info();

	//create environment variable WHALE and initialize to 7
	char whale[] = "WHALE=7";
	if (putenv(whale) != 0) {
		perror("putenv");
		fflush(stderr);
		return errno;
	}

	// create child process 1
	pid_t pid = fork();
	if (pid == 0) {
		// run C1 process
		run_C1();
	} else if (pid < 0) {
		perror("C1 fork error");
	}

	// create child process 2
	pid = fork();
	if (pid == 0) {
		// run C2 process
		run_C2();
	} else if (pid < 0) {
		perror("C2 fork error");
	}

	// run parent process
	run_parent_process();
    return 0;
}
