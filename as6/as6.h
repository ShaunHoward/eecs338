#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#define ATOM_COUNT 25
#define SEM_COUNT 3
#define S 0
#define SC 1
#define SH 2
#define S_VAL 1
#define SC_VAL 0
#define SH_VAL 0
typedef enum { false, true } bool;

// Safely encapsulate posix semaphore
typedef sem_t Semaphore;

// Make easy to use methods to encapsulate sem procedures
Semaphore *make_semaphore(int value);
void semaphore_wait(Semaphore *sem);
void semaphore_signal(Semaphore *sem);

// wrap thread data, just tid
typedef struct _thread_data_t {
	int tid;//thread id
} thread_data_t;

// wrap error checking and exiting with failure
void perror_exit(char *s) {
	perror(s);
	exit(EXIT_FAILURE);
}

// wrap semaphore destruction
void destroy_sem(Semaphore *s){
	int dest_ret_code = sem_destroy(s);
	if (dest_ret_code != 0)
		perror_exit("Error destroying semaphore");
	return;
}

// run malloc for the specified size and error check
void *check_malloc(int size) {
  void *p = malloc (size);
  if (p == NULL)
	  perror_exit("malloc failed");
  return p;
}



Semaphore *make_semaphore(int value) {
  Semaphore *sem = check_malloc(sizeof(Semaphore));
  int sem_ret_code = sem_init(sem, 0, value);
  if (sem_ret_code != 0)
    perror_exit("Error initializing semaphore");
  return sem;
}

// define nice, easy wrapper for sem waiting
void my_sem_wait(Semaphore *sem) {
  int sem_ret_code = sem_wait(sem);
  if (sem_ret_code != 0)
    perror_exit("Error waiting on semaphore");
  return;
}

// define nice, easy wrapper for sem signaling
void my_sem_sig(Semaphore *sem) {
  int sem_ret_code = sem_post(sem);
  if (sem_ret_code != 0)
    perror_exit("Error signaling semaphore");
  return;
}

void print_spawn_atom(bool is_carbon) {
	fflush(stdout);
	if (is_carbon == true)
		printf("Carbon atom is being spawned...\n");
	else
		printf("Hydrogen atom is being spawned...\n");
	fflush(stdout);
	return;
}

void print_thread_barrier(bool is_carbon, int num_waiting_c, int num_waiting_h){
	fflush(stdout);
	if (is_carbon == true)
		printf("Carbon atom has reached barrier...\n");
	else
		printf("Hydrogen atom has reached barrier...\n");
	fflush(stdout);
	printf("Curr number C atoms waiting: %d\n", num_waiting_c);
	printf("Curr number H atoms waiting: %d\n", num_waiting_h);
	fflush(stdout);
	return;
}

void print_full_set(){
	fflush(stdout);
	printf("A full set of C and H threads have crossed the barrier!\n");
	fflush(stdout);
	return;
}
