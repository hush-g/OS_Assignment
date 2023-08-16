#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<errno.h>

#define MAX_COMMANDS 20
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define MAX_HISTORY_SIZE 250

char history[MAX_HISTORY_SIZE][1024];
int historyCount = 0;

void printHistory() {
    for (int i = 0; i < historyCount; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void addHistory(char *command) {
    if (historyCount < MAX_HISTORY_SIZE) {
        strcpy(history[historyCount], command);
        historyCount++;
    } else {
        for (int i = 1; i < historyCount; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[historyCount - 1], command);
    }
}

void processArguments(char *command, char *args[MAX_ARGS]) {
    int arg_count = 0;
    char *token = strtok(command, " ");
    while (token != NULL && arg_count < MAX_ARGS) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

}

void executeCommand(char *args[MAX_ARGS]) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: Missing argument\n");
        } else {
            chdir(args[1]);
        }
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if(strcmp(args[0], "history") == 0) {
        printHistory();
    } else {
        pid_t pid = fork();
        if(pid < 0) {
            printf("\nFork Failed");
        } else if(pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(NULL);
            if (strcmp(args[0], "cat") == 0) {
                printf("\n");
            }
        }
    }
}

void executePipedCommands(char *commands[][1024], int num_commands) {
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];

    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            char *args[MAX_ARGS];
            processArguments(*commands[i], args);
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            pids[i] = pid;
        } else {
            perror("fork");
            return;
        }

        if (i < num_commands - 1) {
            close(pipes[i][1]);
        }
        if (i > 0) {
            close(pipes[i - 1][0]);
        }
    }

    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_commands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
}

int main() {
    char *username = getenv("USER");
    int count = 0;

    while (1) {  
        count++;
        char currentDirectory[1024];
	    getcwd(currentDirectory, sizeof(currentDirectory));
	    printf("%s:%s MTL 458 > ", username, currentDirectory);

        char inputString[MAX_COMMAND_LENGTH];
        int numCommands = 0;
        char *commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
        fgets(inputString, sizeof(inputString), stdin);
        inputString[strcspn(inputString, "\n")] = '\0';

        addHistory(inputString);

        char *token = strtok(inputString, "|");
        while (token != NULL && numCommands < MAX_COMMANDS) {
            commands[numCommands++][0] = token;
            token = strtok(NULL, "|");
        }

        int isPipe = 0;

	    if (numCommands < 2) {
		    isPipe = 0;
	    } else {
		    isPipe = 1;
	    }
        if(isPipe) {
            executePipedCommands(commands, numCommands);
        } else {
            char *args[MAX_ARGS];
            processArguments(*commands[0], args);
            executeCommand(args);
        }
    }
    return 0;
}
