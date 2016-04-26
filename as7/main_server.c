/**
 * The function templates in this code were developed by rpcgen.
 * The functionality was programmed in for as7, eecs 338.
 * @author Shaun Howard
 */

#include "main.h"

// track whether the client data has been initialized
int is_initialized = 0;

// store the current time in an array of 26 chars
char curr_time[26];

// allocate space for each of 3 clients to put 5 messages on the server
struct client_data client_msgs[MSG_LIMIT];

// store the client ids in the same order as client msgs
int client_ids[MSG_LIMIT];

// store the current client msg index, which should be at max equal to MSG_LIMIT
int curr_index = 0;

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

// returns 1 if the given id is in the list, 0 if not
int is_in_id_list(int id){
	int i = 0;
	for (i = 0; i < MSG_LIMIT; i++){
		if (id == client_ids[i]){
			return 1;
		}
	}
	return 0;
}

// Removes the msg at the specified index from the client_msgs array
void remove_from_msg_list(int index){
	if (index > 0 && index < MSG_LIMIT){
		client_msgs[index].id = -1;
	}
}

int *
get_1_svc(int *argp, struct svc_req *rqstp)
{
	// initially, store error result
	static int  result = -1;
	// set the most updated time for the global time array
    set_time();
	printf("[%s] Server received a GET request from client %d.\n", curr_time, *argp);
    fflush(stdout);

	//check if there are any messages received from clients other than the one specified
    int i;
    int client_id = *argp;
    for (i = 0; i < MSG_LIMIT; i++){
        // check if the client id is in the list of ids and not equal to this id
    	if (is_in_id_list(client_msgs[i].id) == 1 && client_msgs[i].id != client_id){
            remove_from_msg_list(i);
            result = 0;
            // break from loop, we have a winner
            break;
    	}
    }

    // check if any messages were found, set success result if so
    if (result >= 0){
    	result = 0;
    }
	return &result;
}

int *
put_1_svc(struct client_data *argp, struct svc_req *rqstp)
{
	static int  result = -1;
	// set the most updated time for the global time array
	set_time();
	printf("[%s] Server received a PUT request from client %d.\n", curr_time, argp->id);
    fflush(stdout);

	if (curr_index < MSG_LIMIT){
	    // store client id
		client_msgs[curr_index].id = argp->id;
		// store id in ordered id list
		client_ids[curr_index] = argp->id;
		// store client msg
	    strcpy(client_msgs[curr_index].message, argp->message);
	    // increment current client msg index
	    curr_index += 1;
	    // set result to 0
	    result = 0;
	}
	return &result;
}
