#include "as5.h"

// Carbon atom - as5 - Shaun Howard

struct common *shared;

int main(int argc, char *argv[]){
	int semid, shmid;
    semid = atoi(argv[1]);
    shmid = atoi(argv[2]);

	int my_pid = getpid();
    print_spawn_atom(true);
	my_sem_wait(semid, S);

	fflush(stdout);
	printf("carbon (C) atom %d is created\n", my_pid);
	fflush(stdout);

	print_process_barrier(true, shared->waiting_c, shared->waiting_h);

	if (shared->waiting_h >= 4) {
		int i;
		for(i = 0; i < 4; i++)
			my_sem_sig(semid, SH);
		shared->waiting_h -= 4;
		my_sem_sig(semid, S);
		print_full_set();
	} else {
		shared->waiting_c += 1;
		my_sem_sig(semid, S);
		my_sem_wait(semid, SC);
	}

	return EXIT_SUCCESS;
}
