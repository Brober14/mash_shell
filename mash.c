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
    

}

