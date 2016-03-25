#include "as5.h"

// Hydrogen atom - as5 - Shaun Howard

struct common *shared;

int main(int argc, char *argv[]){
	int semid, shmid;

	int my_pid = getpid();
	if ((semid = semget(SEM_KEY, SEM_COUNT, IPC_CREAT)) < 0) {
		perror("semget");
		exit(EXIT_FAILURE);
	}
	if ((shmid = shmget(SHM_KEY, 1*K, IPC_CREAT)) < 0) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}
	if ((shared = (struct common *)shmat(shmid, 0, 0)) < 0) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	print_spawn_atom(false);
	my_sem_wait(semid, S);

	fflush(stdout);
	printf("hydrogen (H) atom %d is created\n", my_pid);
	fflush(stdout);

	print_process_barrier(false, shared->waiting_c, shared->waiting_h);

	if (shared->waiting_h >= 3 && shared->waiting_c >= 1) {
		int i;
		for(i = 0; i < 3; i++)
			my_sem_sig(semid, SH);
		shared->waiting_h -= 3;
		my_sem_sig(semid, SC);
		shared->waiting_c -= 1;
		my_sem_sig(semid, S);
		print_full_set();
	} else {
		shared->waiting_h += 1;
		my_sem_sig(semid, S);
		my_sem_wait(semid, SH);
	}

	return EXIT_SUCCESS;
}
