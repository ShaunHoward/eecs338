/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 * NOTE: The shell running this program must have fortune installed.
 */

#include "main.h"
#include <time.h>
#include <unistd.h>
#define NUM_PUTS 5
#define MSG_LIMIT 15

// store the current time in a global array of 26 chars
char curr_time[26];

// sets the current time and date to the "curr_time" char buffer
void set_time() {
	time_t rawtime;
	struct tm * timeinfo;

	// reinitialize curr_time buff if it is null
	if (curr_time == NULL) {
		curr_time[0] = 0;
	}

	// get the current time in ms
	time (&rawtime);
	if (rawtime == ((time_t)-1)) {
		perror("error generating time information...");
		curr_time[0] = 0;
		return;
	}

	// convert time to local time
	timeinfo = localtime (&rawtime);
    if (timeinfo == NULL) {
        perror("error converting time to local time...");
        curr_time[0] = 0;
        return;
    }

    // convert time to asctime
    asctime_r(timeinfo, curr_time);
	if (curr_time == NULL) {
		perror("error converting time to asc time...");
		curr_time[0] = 0;
		return;
	}

	// remove newline from end, just put terminator
	int new_line = strlen(curr_time) - 1;
	if (curr_time[new_line] == '\n') {
		curr_time[new_line] = '\0';
	}
}

// prints the response status from the server given the int code, 0 is success
void print_status(int status) {
	set_time();
    // print success or error message
    if (status == 0) {
        printf ("[%s] Success response from server.\n", curr_time);
    } else {
        printf("[%s] Failure response from server.\n", curr_time);
    }
    fflush(stdout);
}

// uses the fortune program to generate a random string, returns a pointer to that string
char *get_random_string(){
    FILE *fp;

    // variables for creating random string
    char *random_str = NULL;
    char *temp = NULL;

    // use a temporary buffer to incrementally gather results
    char buf[100];

    // maintain size variables for dynamically adjusting the string size
    unsigned int size = 1;
    unsigned int str_length;

    // call fortune to generate a real random string
    fp = popen("fortune", "r");

    // error-check fortune teller response
    if (fp == NULL) {
        perror("Error running fortune to generate random string...");
	    fflush(stderr);
	    if (pclose(fp) == -1){
	        perror("Error closing fortune process...");
	        fflush(stderr);
	    }
	    return "error generating random string...";
    }

    // attain your fortune by writing all string characters to random_str incrementally
    while (fgets(buf, sizeof(buf), fp) != NULL) {
    	// get the current buf size
    	str_length = strlen(buf);
    	// dynamically reallocate the random string with a new size
        temp = realloc(random_str, size + str_length);
        // error-check reallocation
    	if (temp == NULL) {
        	perror("temporary random string memory allocation failed...");
            return "";
        } else {
        	// set the random string on success
        	random_str = temp;
        }
    	// copy the buffer to the random string
        if (strcpy(random_str + size - 1, buf) == NULL){
        	perror("error copying random string...");
        }
        // increase the size variable by the added string length
        size += str_length;
    }

    // error-check closing fortune process
	if (pclose(fp) == -1){
	    perror("Error closing fortune process...");
	    return "error closing fortune process...";
	}
    return random_str;
}

void
display_prg_1(char *host)
{
	CLIENT *clnt;
	// seed random with null
	srand(time(NULL));
	// get random number between 0 and 100
	int host_id = (int)(rand() % (100 + 1 - 0) + 0);
	int  *result_1;
	int  get_1_arg;
	int  *result_2;
	struct client_data  put_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, DISPLAY_PRG, DISPLAY_VER, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	int i;
	// client puts random message to server NUM_PUTS times
	for (i = 0; i < NUM_PUTS; i++) {
	    // set host id in message
        put_1_arg.id = host_id;
		// generate and set random string for message
        char *random_str = get_random_string();
        if (strcpy(put_1_arg.message, random_str) == NULL){
        	perror("error copying put message...");
        }
        // free the string pointer
        free(random_str);
        // set global time
        set_time();

        // print put request message
	    printf ("[%s] Client %d sent a PUT request.\n", curr_time, host_id);
	    fflush(stdout);

        // put generated message with id onto server
		result_2 = put_1(&put_1_arg, clnt);
		// signal error if occurs
	    if (result_2 == (int *) NULL) {
		    clnt_perror (clnt, "call failed");
	    }

	    // print the status of the server response
        print_status(*result_2);

	    // sleep one second between puts
	    sleep(1);
	}

	// sleep a total of 5 seconds
	// sleep 4 more seconds (already slept 1 second from last iteration of loop)
    sleep(4);

    // client calls get from server 2*NUM_PUTS times
    for (i = 0; i < 2*NUM_PUTS; i++) {
    	// set host id in message
        get_1_arg = host_id;
        // set curr time
    	set_time();
    	// print get request message
    	printf("[%s] Client %d sent a GET request.\n", curr_time, host_id);
    	fflush(stdout);

    	// call get on server
    	result_1 = get_1(&get_1_arg, clnt);
    	if (result_1 == (int *) NULL) {
    		clnt_perror (clnt, "call failed");
    	}

    	// print the status of the server response
        print_status(*result_1);
    	// sleep one second between gets
    	sleep(1);
    }

#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	display_prg_1 (host);
    exit (0);
}
