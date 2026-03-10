#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define MAX_ARGS            64

#define DELIM               "\n\t\a "

void parse_command(char *command, char *args[MAX_ARGS]) {

    int count = 0;
    char *token = strtok(command, DELIM);
    while(token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, DELIM);
    }
    args[count] = NULL;
}

int execute_command(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if(pid == 0) {
        if(execvp(args[0], args) == -1) {
            perror("MyShell");
            fflush(stderr);
        }
        exit(EXIT_FAILURE);
    } else if(pid < 0) {
        perror("MyShell");
        fflush(stderr);
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int execute_builtin_or_external(char **args) {
    if(args[0] == NULL)
        return 1;

    if(strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if(strcmp(args[0], "cd") == 0) {
        if(args[1] == NULL) {
            printf("Введите больше аргументов для команды cd\n");
            fflush(stdout);
        } else {
            if (chdir(args[1]) != 0)
                perror("shell");
        }
    } else {
        execute_command(args);
    }

    return 1;
}

int main() {

    char* command = NULL;
    size_t size = 0;
    char *args[64];

    while(1) {
        printf("Myshell> ");
        fflush(stdout);

        if(getline(&command, &size, stdin) == -1) {
            printf("Input error");
            fflush(stdout);
        }

        parse_command(command, args);

        execute_builtin_or_external(args);
        
    }

    free(command);
    return 0;
}