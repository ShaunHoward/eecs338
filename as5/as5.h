#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define SEM_KEY 100
#define SHM_KEY 100
#define K 1024
#define ATOM_COUNT 25
#define SEM_COUNT 3
#define S 0
#define SC 1
#define SH 2
#define S_VAL 1
#define SC_VAL 0
#define SH_VAL 0
typedef enum { false, true } bool;


// initialize semaphores
union semin {
	unsigned short *array;
};

// define the common shared memory
struct common {
	int waiting_c;
	int waiting_h;
};

void print_spawn_atom(bool is_carbon) {
	fflush(stdout);
	if (is_carbon == true)
		printf("carbon atom is being spawned\n");
	else
		printf("hydrogen atom is being spawned\n");
	fflush(stdout);
	return;
}

void print_process_barrier(bool is_carbon, int num_waiting_c, int num_waiting_h){
	fflush(stdout);
	if (is_carbon == true)
		printf("carbon atom has reached barrier\n");
	else
		printf("hydrogen atom has reached barrier\n");
	fflush(stdout);
	printf("curr number C atoms waiting: %d\n", num_waiting_c);
	fflush(stdout);
	printf("curr number H atoms waiting: %d\n", num_waiting_h);
	fflush(stdout);
	return;
}

void print_full_set(){
	fflush(stdout);
	printf("a full set of C and H processes have crossed the barrier!\n");
	fflush(stdout);
	return;
}

void my_sem_wait(int semid, int sem_val) {
	struct sembuf sem_wait_buf;
	sem_wait_buf.sem_op = -1;
	sem_wait_buf.sem_flg = 0;
	sem_wait_buf.sem_num = sem_val;
	semop(semid, &sem_wait_buf, 1);
	return;
}

void my_sem_sig(int semid, int sem_val) {
	struct sembuf sem_sig_buf;
	sem_sig_buf.sem_op = 1;
	sem_sig_buf.sem_flg = 0;
	sem_sig_buf.sem_num = sem_val;
	semop(semid, &sem_sig_buf, 1);
	return;
}
