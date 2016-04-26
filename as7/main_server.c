/**
 * The function templates in this code were developed by rpcgen.
 * The functionality was programmed in for as7, eecs 338.
 * @author Shaun Howard
 */

#include "main.h"

// track whether the client data has been initialized
int is_initialized = 0;

// allocate space for each of 3 clients to put 5 messages on the server
struct client_data client_msgs[MSG_LIMIT];
int i = 0;

void init_client_data(){
	for (i=0; i< sizeof(client_msgs); i++) {
		client_msgs[i].id = -1;
	}
}


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

	time (&rawtime);
	if (rawtime == ((time_t)-1)) {
		perror("error generating time information...");
		curr_time[0] = 0;
		return;
	}

	timeinfo = localtime (&rawtime);
    if (timeinfo == NULL) {
        perror("error converting time to local time...");
        curr_time[0] = 0;
        return;
    }
    asctime_r(timeinfo, curr_time);
	if (curr_time == NULL) {
		perror("error converting time to asc time...");
		curr_time[0] = 0;
		return;
	}
	int new_line = strlen(curr_time) - 1;
	if (curr_time[new_line] == '\n') {
		curr_time[new_line] = '\0';
	}
}

int *
get_1_svc(int *argp, struct svc_req *rqstp)
{
	// initialize client data if not already done
	if (is_initialized == 0) {
		is_initialized += 1;
		init_client_data();
	}
	// initially, store error result
	static int  result = -1;
    set_time();
	printf("[%s] Server received a GET request from client %d.\n", curr_time, *argp);
    fflush(stdout);

	//check if there are any messages received from clients other than the one specified
    int i;
    for (i = 0; i < sizeof(client_msgs); i++){
    	if (client_msgs[i].id != -1 && client_msgs[i].id != *argp){
    		printf("checked here once");
    		result += 1;
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
	// initialize client data if not already dones
//	if (is_initialized == 0) {
//		is_initialized += 1;
//		init_client_data();
//	}

	static int  result = -1;
	set_time();
	printf("[%s] Server received a PUT request from client %d.\n", curr_time, argp->id);
    fflush(stdout);

	if (curr_index < MSG_LIMIT){
	    // store client id and message
		client_msgs[curr_index].id = argp->id;
	    strcpy(client_msgs[curr_index].message, argp->message);
	    // increment current client msg index
	    curr_index += 1;
	    // set result to 0
	    result = 0;
	}
	return &result;
}