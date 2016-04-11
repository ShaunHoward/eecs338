#include "as6.h"

// main - as6 - Shaun Howard

// seed random with this
time_t t;

// allocate semaphore array
Semaphore* sem[SEM_COUNT];

// initialize shared memory
int waiting_c = 0;
int waiting_h = 0;

// used to execute carbon atom pthread
void *carbon(void *arg) {
	// get thread id
	thread_data_t *data = (thread_data_t *)arg;
	int tid = data->tid;

	// print that the atom spawned
    print_spawn_atom(true);

	fflush(stdout);
	printf("carbon (C) atom %d is created\n", tid);
	fflush(stdout);

    // wait for mutex
	my_sem_wait(sem[S]);

	// print that the thread has reached the barrier and the
	// curr waiting c and h atom counts
	print_thread_barrier(true, waiting_c, waiting_h);

	// are there enough h atoms waiting to make CH4?
	if (waiting_h >= 4) {
		int i;
		for(i = 0; i < 4; i++)
			my_sem_sig(sem[SH]);
		waiting_h -= 4;
		my_sem_sig(sem[S]);
		print_full_set();
	} else {
		// if not enough h atoms, carbon has to wait
		waiting_c += 1;
		my_sem_sig(sem[S]);
		my_sem_wait(sem[SC]);
	}
	// exit pthread with success
	pthread_exit(NULL);
}

// used to execute hydrogen atom pthread
void *hydrogen(void *arg) {
	// get thread id
	thread_data_t *data = (thread_data_t *)arg;
	int tid = data->tid;

	// print that atom spawned
	print_spawn_atom(false);

	fflush(stdout);
	printf("hydrogen (H) atom %d is created\n", tid);
	fflush(stdout);

	// wait for mutex
	my_sem_wait(sem[S]);

	// print that atom is at thread barrier
	print_thread_barrier(false, waiting_c, waiting_h);

	// are there enough h and c atoms to make CH4?
	if (waiting_h >= 3 && waiting_c >= 1) {
		int i;
		for(i = 0; i < 3; i++)
			my_sem_sig(sem[SH]);
		waiting_h -= 3;
		my_sem_sig(sem[SC]);
		waiting_c -= 1;
		my_sem_sig(sem[S]);
		print_full_set();
	} else {
		// if not enough c or h atoms, hydrogen has to wait
		waiting_h += 1;
		my_sem_sig(sem[S]);
		my_sem_wait(sem[SH]);
	}

	// exit successfully
	pthread_exit(NULL);
}

int main() {
	// allocate space for initial threads and thread data
    pthread_t initial_threads[ATOM_COUNT];
	thread_data_t thread_data[ATOM_COUNT];

	// seed random
	srand((unsigned)time(&t));

    // initialize the semaphores using predefined values for the sequential CH4 prod. problem
	// use helper function to initialize semaphores
	sem[S] = make_semaphore(S_VAL);
	sem[SH] = make_semaphore(SH_VAL);
	sem[SC] = make_semaphore(SC_VAL);

    // count number of carb and hydro atoms randomly spawned
	int carb_count = 0;
	int hydro_count = 0;

	// do random execution
	int i;
	int ret_code = 0;
	for (i = 0; i < ATOM_COUNT; i++) {
		// set thread id
		thread_data[i].tid = i;
		int val = rand();
		// spawn carbon atom when rand odd
		if (val % 2 != 0) {
			if ((ret_code = pthread_create(&initial_threads[i], NULL, carbon, &thread_data[i]))) {
			    fflush(stderr);
				fprintf(stderr, "error: C pthread_create, %d\n", ret_code);
				fflush(stderr);
			    return EXIT_FAILURE;
			}
			carb_count++;
		} else {
			// otherwise, spawn hydrogen atom when rand even
			if ((ret_code = pthread_create(&initial_threads[i], NULL, hydrogen, &thread_data[i]))) {
			    fflush(stderr);
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

	// allocate more space for extra h threads and thread data
	pthread_t extra_h_threads[num_hydro_left];
	thread_data_t extra_thread_data[num_hydro_left];

	// compensate for mismatched spawning with extra hydrogen
	for (i = 0; i < num_hydro_left; i++) {
		fflush(stdout);
		printf("Spawning extra hydrogen...\n");
		fflush(stdout);
		// set thread id
		extra_thread_data[i].tid = i+ATOM_COUNT;

		// spawn hydrogen atom
		if ((ret_code = pthread_create(&extra_h_threads[i], NULL, hydrogen, &extra_thread_data[i]))) {
		    fflush(stderr);
			fprintf(stderr, "error: H pthread_create, %d\n", ret_code);
		    fflush(stderr);
			return EXIT_FAILURE;
		}
	}

	int total_atom_count = ATOM_COUNT + num_hydro_left;
	int j = 0;
    // wait for all atoms to be destroyed, even though science states that's impossible
    for (i = 0; i < total_atom_count; i++) {
    	fflush(stdout);
    	printf("Waiting for thread %d to exit...\n", i);
    	fflush(stdout);
    	// destroy initial threads
    	if (i < ATOM_COUNT){
    		if ((ret_code = pthread_join(initial_threads[i], NULL))) {
    			fflush(stderr);
    			fprintf(stderr, "error: initial pthread_join, id: %d, errcode: %d\n", i, ret_code);
    			fflush(stderr);
    		}
    	} else {
    		// destroy additional hydrogen threads
    		// adjust index for additional thread array
    		j = i - ATOM_COUNT;
			if ((ret_code = pthread_join(extra_h_threads[j], NULL))) {
				fflush(stderr);
				fprintf(stderr, "error: additional H pthread_join, id: %d, errcode: %d\n", j, ret_code);
				fflush(stderr);
			}
    	}
    }

    fflush(stdout);
    printf("Sequential CH4 making complete. Semaphores are being removed.\n");
    fflush(stdout);

    // destroy the semaphores and free memory
    destroy_sem(sem[S]);
    free(sem[S]);
    destroy_sem(sem[SC]);
    free(sem[SC]);
    destroy_sem(sem[SH]);
    free(sem[SH]);

    fflush(stdout);
    printf("Exiting sequential CH4 process...\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
