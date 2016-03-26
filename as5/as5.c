#include "as5.h"

// main - as5 - Shaun Howard

// seed random with this
time_t t;

// used to execute carbon atom process
void carbon(int semid, int shmid) {
	// convert the semid and shmid into characters
    char semidbuf[1];
    char shmidbuf[1];
    sprintf(semidbuf, "%d", semid);
    sprintf(shmidbuf, "%d", shmid);

    // run carbon with semid and shmid
    execl("carbon", "carbon", semidbuf, shmidbuf, NULL);

    // error when exec returns
    perror("execl");
    exit(EXIT_FAILURE);
}

// used to execute hydrogen atom process
void hydrogen(int semid, int shmid) {
	// convert the semid and shmid into characters
	char semidbuf[1];
    char shmidbuf[1];
    sprintf(semidbuf, "%d", semid);
    sprintf(shmidbuf, "%d", shmid);

    // run hydrogen with semid and shmid
	execl("hydrogen", "hydrogen", semidbuf, shmidbuf, NULL);

	// error if exec returns
	perror("execl");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int semid, shmid;
	unsigned short sem_vals[SEM_COUNT];
	struct common *shared;

	// initialize semaphore space
	union semin semctlarg;
	// seed random
	srand((unsigned)time(&t));

	if ((semid = semget(IPC_PRIVATE, SEM_COUNT, 0777)) < 0) {
		perror("semget_main");
		exit(EXIT_FAILURE);
	}

    // initialize semaphore values
	sem_vals[S] = S_VAL;
	sem_vals[SH] = SH_VAL;
	sem_vals[SC] = SC_VAL;
	semctlarg.array = sem_vals;

    // initialize actual semaphores
	if ((semctl(semid, SEM_COUNT, SETALL, semctlarg)) < 0) {
		perror("semctl_main_1");
		exit(EXIT_FAILURE);
	}

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
	int ret_val = 0;
	for (i = 0; i < ATOM_COUNT; i++) {
		// spawn carbon atom when rand odd
		int val = rand();
		if (val % 2 != 0) {
			if ((ret_val = fork()) == 0){
				carbon(semid, shmid);
			} else if (ret_val < 0) {
				perror("fork");
				exit(EXIT_FAILURE);
			} else {
				carb_count++;
			}
		} else {
			// otherwise, spawn hydrogen atom when rand even
			if ((ret_val = fork()) == 0)
				hydrogen(semid, shmid);
			else if (ret_val < 0){
				perror("fork");
				exit(EXIT_FAILURE);
			} else {
				hydro_count++;
			}
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

	// compensate for mis-matched spawning with extra hydrogen
	for (i = 0; i < num_hydro_left; i++) {
		printf("Spawning extra hydrogen...\n");
		fflush(stdout);
		// spawn hydrogen atom
		if ((ret_val = fork()) == 0){
			hydrogen(semid, shmid);
		} else if (ret_val < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
	}

	int total_atom_count = ATOM_COUNT + num_hydro_left;

    // wait for all atoms to be destroyed, even though science states that's impossible
    for (i = 0; i < total_atom_count; i++) {
    	fflush(stdout);
    	printf("Waiting for process %d to exit...\n", i);
    	fflush(stdout);
    	if (wait(0) < 0) {
    		perror("wait");
    		exit(EXIT_FAILURE);
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
