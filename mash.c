#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    //Run the shell loop
    mash_loop();

    //Exit the program
    return EXIT_SUCCESS;
}


void mash_loop() {
    
    char *line;
    char **args;
    int status;

    do {

        printf("mash: >>");
        line = mash_read_line();
        args = mash_split_line(line);
        status = mash_execute();

        //Garbage Collection
        free(line);
        free(args);
    }while(status)
}

#define MASH_RL_BUFFSIZE
char *mash_read_line() {
    int buffsize = MASH_RL_BUFFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffsize)
    int c;

    if(!buffer) {
        fprintf(stderr, "mash: allocation error");
        exit(EXIT_FAILURE);
    }

    while(1) {
        //Assign character in int to c variable
        c = getchar();

        //Check if we hit EOF or newline character
        if(c == EOF || c == "\n"){
            buffer[position] = "\0";
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        //Reallocation of memory if buffersize is exceeded
        if(position > buffsize) {
            buffsize += MASH_RL_BUFFSIZE;
            buffer = realloc(buffer, buffsize);
            if(!buffer){
                fprintf(stderr, "mash: allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }

}



