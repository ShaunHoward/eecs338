#include "as6.h"

// main - as5 - Shaun Howard

// seed random with this
time_t t;

// initialize shared memory and semaphores
struct common *shared;
Semaphore* sem[SEM_COUNT];

// used to execute carbon atom process
void *carbon(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;

    print_spawn_atom(true);
	my_sem_wait(sem[S]);

	int my_pid = getpid();
	fflush(stdout);
	printf("carbon (C) atom %d is created\n", my_pid);
	fflush(stdout);

	print_process_barrier(true, shared->waiting_c, shared->waiting_h);

	// are there enough h atoms waiting to make CH4?
	if (shared->waiting_h >= 4) {
		int i;
		for(i = 0; i < 4; i++)
			my_sem_sig(sem[SH]);
		shared->waiting_h -= 4;
		my_sem_sig(sem[S]);
		print_full_set();
	} else {
		// if not enough h atoms, carbon has to wait
		shared->waiting_c += 1;
		my_sem_sig(sem[S]);
		my_sem_wait(sem[SC]);
	}
	exit(EXIT_SUCCESS);
}

// used to execute hydrogen atom process
void *hydrogen(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;

	print_spawn_atom(false);
	my_sem_wait(sem[S]);

	int my_pid = getpid();
	fflush(stdout);
	printf("hydrogen (H) atom %d is created\n", my_pid);
	fflush(stdout);

	print_process_barrier(false, shared->waiting_c, shared->waiting_h);

	// are there enough h and c atoms to make CH4?
	if (shared->waiting_h >= 3 && shared->waiting_c >= 1) {
		int i;
		for(i = 0; i < 3; i++)
			my_sem_sig(sem[SH]);
		shared->waiting_h -= 3;
		my_sem_sig(sem[SC]);
		shared->waiting_c -= 1;
		my_sem_sig(sem[S]);
		print_full_set();
	} else {
		// if not enough c or h atoms, hydrogen has to wait
		shared->waiting_h += 1;
		my_sem_sig(sem[S]);
		my_sem_wait(sem[SH]);
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
	int semid, shmid;
    pthread_t initial_threads[ATOM_COUNT];

	// seed random
	srand((unsigned)time(&t));

    // initialize the semaphores using predefined values for the sequential CH4 prod. problem
	// use helper function to initialize semaphores
	sem[S] = make_semaphore(S_VAL);
	sem[SH] = make_semaphore(SH_VAL);
	sem[SC] = make_semaphore(SC_VAL);

	// find memory id for shared
	if ((shmid = shmget(IPC_PRIVATE, 1*K, 0777)) < 0) {
		perror("shmget_main");
		exit(EXIT_FAILURE);
	}

	// get shared memory pointer
	if ((shared = (struct common *)shmat(shmid, 0, 0)) < 0) {
		perror("shmat_main");
		exit(EXIT_FAILURE);
	}

	// initialize shared memory
	shared->waiting_c = 0;
	shared->waiting_h = 0;

    // count number of carb and hydro atoms randomly spawned
	int carb_count = 0;
	int hydro_count = 0;

	// do random execution
	int i;
	int ret_code = 0;
	for (i = 0; i < ATOM_COUNT; i++) {
		// spawn carbon atom when rand odd
		int val = rand();
		if (val % 2 != 0) {
			if ((ret_code = pthread_create(&initial_threads[i], NULL, carbon, i))) {
			    fprintf(stderr, "error: C pthread_create, %d\n", ret_code);
				fflush(stderr);
			    return EXIT_FAILURE;
			}
			carb_count++;
		} else {
			// otherwise, spawn hydrogen atom when rand even
			if ((ret_code = pthread_create(&initial_threads[i], NULL, hydrogen, i))) {
			    fprintf(stderr, "error: H pthread_create, %d\n", ret_code);
			    fflush(stderr);
				return EXIT_FAILURE;
			}
			hydro_count++;
		}
	}

	// determine number of carbon atoms already satisfied
	int num_carb_satisfied = hydro_count / 4;
	// determine hydrogen left over from previous spawn loop
	int hydro_left_over = hydro_count % 4;
	// determine number of carbon left to satisfy
	int num_carb_left = carb_count - num_carb_satisfied;
	// determine number of hydrogen needed to satisfy all carbons
	int num_hydro_left = (num_carb_left * 4) - hydro_left_over;
	pthread_t extra_h_threads[num_hydro_left];

	// compensate for mis-matched spawning with extra hydrogen
	for (i = 0; i < num_hydro_left; i++) {
		printf("Spawning extra hydrogen...\n");
		fflush(stdout);
		// spawn hydrogen atom
		if ((ret_code = pthread_create(&initial_threads[i], NULL, hydrogen, i))) {
		    fprintf(stderr, "error: H pthread_create, %d\n", ret_code);
		    fflush(stderr);
			return EXIT_FAILURE;
		}
	}

	int total_atom_count = ATOM_COUNT + num_hydro_left;

    // wait for all atoms to be destroyed, even though science states that's impossible
    for (i = 0; i < total_atom_count; i++) {
    	fflush(stdout);
    	printf("Waiting for process %d to exit...\n", i);
    	fflush(stdout);
    	if (i < ATOM_COUNT){
    		if ((ret_code = pthread_join(initial_threads[i], NULL))) {
    			fprintf(stderr, "error: initial pthread_join, %d\n", ret_code);
    		}
    	} else {
			if ((ret_code = pthread_join(extra_h_threads[i], NULL))) {
				fprintf(stderr, "error: additional H pthread_join, %d\n", ret_code);
			}
    	}
    }

    fflush(stdout);
    printf("Sequential CH4 making complete. Shared memory and semaphores are being removed.\n");
    fflush(stdout);

    // remove shared memory
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
    	perror("shmctl_main");
    	exit(EXIT_FAILURE);
    }

    // remove semaphores
    if (semctl(semid, SEM_COUNT, IPC_RMID, 0) < 0) {
    	perror("semctl_main_2");
    	exit(EXIT_FAILURE);
    }

    fflush(stdout);
    printf("Exiting sequential CH4 program...\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
