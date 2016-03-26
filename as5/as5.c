#include "as5.h"

// main - as5 - Shaun Howard

// seed random with this
time_t t;

void dec_to_str (char* str, int val, size_t digits)
{
      size_t i=1u;

        for(; i<=digits; i++)
              {
                      str[digits-i] = (char)((val % 10u) + '0');
                          val/=10u;
                            }

          str[i-1u] = '\0'; // assuming you want null terminated strings?
}

// used to execute carbon atom process
void carbon(int semid, int shmid) {
    char semidbuf[1];
    char shmidbuf[1];
    dec_to_str(semidbuf, semid, 1);
    dec_to_str(shmidbuf, semid, 1);
    execl("carbon", "carbon", semidbuf, shmidbuf, NULL);
    perror("execl");
    // error when exec returns
    exit(EXIT_FAILURE);
}

// used to execute hydrogen atom process
void hydrogen(int semid, int shmid) {
    char semidbuf[1];
    char shmidbuf[1];
    dec_to_str(semidbuf, semid, 1);
    dec_to_str(shmidbuf, semid, 1);

	execl("hydrogen", "hydrogen", semidbuf, shmidbuf, NULL);
	perror("execl");
	// error if exec returns
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
	int ret_val = 0;
	int i;
	int carb_count = 0;
	int hydro_count = 0;
	int atom_count = ATOM_COUNT;

	// do random execution
	for (i = 0; i < atom_count; i++) {
		// spawn carbon atom when rand even
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
			// otherwise, spawn hydrogen atom when rand odd
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

	int num_carb_satisfied = hydro_count / 4;
	int hydro_left_over = hydro_count % 4;
	int num_carb_left = carb_count - num_carb_satisfied;
	int num_hydro_left = (num_carb_left * 4) - hydro_left_over;

	// compensate for mis-matched spawning
	for (i = 0; i < num_hydro_left; i++) {
		printf("making another hydrogen\n");
		fflush(stdout);
		// spawn hydrogen atom
		if ((ret_val = fork()) == 0){
			hydrogen(semid, shmid);
		} else if (ret_val < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
	}

	atom_count += num_hydro_left;

    // wait for all atoms to be destroyed, even though science states that's impossible
    for (i = 0; i < atom_count; i++) {
    	fflush(stdout);
    	printf("waiting for process %d to exit...\n", i);
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
