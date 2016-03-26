#include "as5.h"

// Carbon atom - as5 - Shaun Howard

struct common *shared;

int main(int argc, char *argv[]){
    int semid = atoi(argv[1]);
    int shmid = atoi(argv[2]);

	int my_pid = getpid();
	if ((shared = (struct common *)shmat(shmid, 0, 0)) < 0) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

    print_spawn_atom(true);
	my_sem_wait(semid, S);

	fflush(stdout);
	printf("carbon (C) atom %d is created\n", my_pid);
	fflush(stdout);

	print_process_barrier(true, shared->waiting_c, shared->waiting_h);

	// are there enough h atoms waiting to make CH4?
	if (shared->waiting_h >= 4) {
		int i;
		for(i = 0; i < 4; i++)
			my_sem_sig(semid, SH);
		shared->waiting_h -= 4;
		my_sem_sig(semid, S);
		print_full_set();
	} else {
		// if not enough h atoms, carbon has to wait
		shared->waiting_c += 1;
		my_sem_sig(semid, S);
		my_sem_wait(semid, SC);
	}

	return EXIT_SUCCESS;
}
