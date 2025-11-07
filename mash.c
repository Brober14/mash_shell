#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>



#define MASH_RL_BUFFSIZE 1024
char *mash_read_line(void) {
    int buffsize = MASH_RL_BUFFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffsize);
    int c;

    if(!buffer) {
        fprintf(stderr, "mash: allocation error");
        exit(EXIT_FAILURE);
    }

    while(1) {
        //Read a character
        c = getchar();

        //Check if we hit EOF or newline character
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        //Reallocation of memory if buffersize is exceeded
        if(position >= buffsize) {
            buffsize += MASH_RL_BUFFSIZE;
            buffer = realloc(buffer, buffsize);
            if(!buffer){
                fprintf(stderr, "mash: allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
}


#define MASH_TOK_BUFFSIZE 64
#define MASH_TOK_DELIMS " \t\r\n\a"
char **mash_split_line(char *line) {
    int buffsize =  MASH_TOK_BUFFSIZE, position = 0;
    char **tokens = malloc(buffsize * sizeof(char));
    char *token;

    if(!tokens){
        fprintf(stderr, "mash: allocation error");
        exit(EXIT_FAILURE);
    }

    //Check for tokens in line and add them to the tokens array
    token = strtok(line, MASH_TOK_DELIMS);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        if(position >= buffsize){
            buffsize += MASH_TOK_BUFFSIZE;
            tokens = realloc(tokens, buffsize);
            if(!tokens){
                fprintf(stderr, "mash: allocation error");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, MASH_TOK_DELIMS);
    }

    //Put the NULL identifier to the end of array for an exec function
    tokens[position] = NULL;
    return tokens;
}

int mash_launch(char **args) {
    pid_t pid, wpid;
    int status;

    //Fork the process for the command execution
    pid = fork();
    if(pid == 0){
        //Child proccess
        if(execvp(args[0], args) == -1) {
            perror("mash");
        }
        exit(EXIT_FAILURE);
    } else if(pid < 0){
        //Fork error
        perror("mash");
    }else {
        //Parent process waits for the child process to finish running or killed
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// Function declaration for built_in commands

int mash_cd(char **args);
int mash_help(char **args);
int mash_exit(char **args);

//List of built_in commands
char *builtin_list[] = {
    "cd",
    "help",
    "exit"
};

//Array of function pointers
int (*builtin_func[]) (char **args) = {
    &mash_cd,
    &mash_help,
    &mash_exit
};

//Function enumerator
int mash_builtin_enum() {
    return sizeof(builtin_list) / sizeof(char *);
}

int mash_cd(char **args) {
    if(args[1] == NULL){
        fprintf(stderr, "mash: expected argument for \"cd\"\n");
    } else {
        if(chdir(args[1]) != 0) {
            perror("mash");
        }
    }

    return 1;
}

int mash_help(char **args) {
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following commands are builtin:\n");

    for (int i = 0; i < mash_builtin_enum(); i++) {
        printf(" %s\n", builtin_list[i]);
    }

    printf("Use man to get information on other programs\n");
    return 1;
}

int mash_exit(char **args) {
    return 0;
}


int mash_execute(char **args) {
    
    if(args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < mash_builtin_enum(); i++) {
        if(strcmp(args[0], builtin_list[i]) == 0) {
            builtin_func[i] (args);
            return 1;
        }
    }

    return mash_launch(args);
}

void mash_loop() {
    
    char *line;
    char **args;
    int status;

    do {

        printf(">> ");
        line = mash_read_line();
        args = mash_split_line(line);
        status = mash_execute(args);

        //Garbage Collection
        free(line);
        free(args);
    } while(status);
}

int main(int argc, char **argv) {
    //Run the shell loop
    mash_loop();

    //Exit the program
    return EXIT_SUCCESS;
}


