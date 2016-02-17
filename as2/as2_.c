#include <sys/types.h>

#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <string.h>


#define BUFFER_SIZE 1024

#define ALPHA_OFFSET 97 // ascii code for "a"

#define LETTERS 26 // letters in the alphabet

#define NUMBER_OF_MAPPERS 4

#define NUMBER_OF_REDUCERS 26

#define READ_END 0

#define WRITE_END 1


void child(int);

void parent(int);



int mapper_pipes[NUMBER_OF_MAPPERS] [2];

int reducer_pipes[NUMBER_OF_REDUCERS][2];



//Reads its input data from a pipe (using system call read()) and

//inspects the characters. For each lowercase letter, the letter is passed to the appropriate reader via a

//pipe (using system call write()).

void mapper(int currentM, int mapperpipe)

{

    int i;

    char buffer[BUFFER_SIZE];


    for(i = currentM+1; i < NUMBER_OF_MAPPERS; i++)

    {

        close(mapper_pipes[i][READ_END]);

    }

    //reads the mapper line from the pipe into another buffer size 1024.

    i = 0;


    if(read(mapper_pipes[currentM][READ_END], buffer, BUFFER_SIZE) < 0) {

        //error

    }


    printf("Mapper %i has buffer %s\n", currentM+1, buffer);

    fflush(stdout);


    //Sends lowercase letters to reducer

    for(i = 0; i < strlen(buffer); i++) {

        if(buffer[i] >= ALPHA_OFFSET && buffer[i] < ALPHA_OFFSET + LETTERS) {

            if(write(reducer_pipes[buffer[i]-ALPHA_OFFSET][WRITE_END], buffer[i], 1) < 0) {

                //error

            }

        }

    }

    

    for(i = 0; i < NUMBER_OF_REDUCERS; i++)

    {

        close(reducer_pipes[i][WRITE_END]);

    }


}


//Counts all instances of each character

void reducer(int currentR, int pipe_read_fd)

{

    int i;

    for(i = 0; i < NUMBER_OF_MAPPERS; i++)

    {

        close(mapper_pipes[i][WRITE_END]);

    }

    


    char buffer[BUFFER_SIZE];


    //Close all but current reducer's read pipe ends.

    for(i = 0; i < NUMBER_OF_REDUCERS; i++)

    {

        if(i != currentR) {

            close(reducer_pipes[i][READ_END]);

        }

    }


    i = 0;

   // int count = 0;


    while(i < BUFFER_SIZE && read(pipe_read_fd, buffer[i], 1) > 0)

    {

        i++;

    }


    close(reducer_pipes[currentR][READ_END]);

    printf("count %c: %d\n", currentR+ALPHA_OFFSET, i);

    fflush(stdout);

}

int main() {


    // Create the mapper pipes

    // Add "for" loop for each of these two

    if(pipe(mapper_pipes)) {

       perror("pipe");

       exit(EXIT_FAILURE);

    }


    // Create the reducer pipes

    if(pipe(reducer_pipes)) {

       perror("pipe");

       exit(EXIT_FAILURE);

    }


   int i;




    pid_t mapperpids[NUMBER_OF_MAPPERS];

    pid_t reducerpids[NUMBER_OF_REDUCERS];


   //Opens and checks the input file for error 

    FILE *input_file = fopen("input.txt", "r");

    if(input_file == NULL)

    {

        perror("fopen");

    }


    i = 0;


    //Writes each line of the input file to each mapper.

    char buffer[BUFFER_SIZE];

    while(fgets(buffer, BUFFER_SIZE, input_file) != NULL)

    {

        printf("line: %s\n", buffer);

        write(mapper_pipes[i][WRITE_END], buffer, strlen(buffer));

        close(mapper_pipes[i][WRITE_END]);

        i++;

    }


    //Closes the input file and checks for error.

   fclose(input_file);

    if(input_file != 0)

    {

        perror("fclose");

    }



    // Fork mappers

    for(i = 0; i < NUMBER_OF_MAPPERS; i++)

    {

        pid_t mapperpid = fork();

        if(mapperpid < 0)

        {

            perror("fork");

            exit(EXIT_FAILURE);

        }


        //if child

        else if(mapperpid == 0)

        {

            close(mapper_pipes[i][WRITE_END]);

            mapper(i, mapper_pipes[i][READ_END]);

            exit(EXIT_SUCCESS);

        }


        //else if parent

        else if(mapperpid > 0)

        {

            close(mapper_pipes[i][READ_END]);


            //current mapper pid is added to a list of mapper pids

            mapperpids[i] = mapperpid;

        }

    }

       //fork reducers

    for(i = 0; i < NUMBER_OF_REDUCERS; i++)

    {

        pid_t reducerpid = fork();

        if(reducerpid < 0) {

            perror("fork");

            exit(EXIT_FAILURE);

        }


        //if child

        else if(reducerpid == 0) {

            reducer(i, reducer_pipes[i][READ_END]);

            exit(EXIT_SUCCESS);

        }


        //if parent

        else if(reducerpid > 0) {

            close(reducer_pipes[i][WRITE_END]);


            //current reducer pid is added to a list of reducer pids

            reducerpids[i] = reducerpid;

        }

    

    }


    for(i = 0; i < sizeof(mapperpids); i++)

    {

        wait(&mapperpids[i]);

    }


    for(i = 0; i < sizeof(reducerpids); i++)

    {

        wait(&reducerpids[i]);

    }


    exit(EXIT_SUCCESS);
    }