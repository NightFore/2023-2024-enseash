// TP1_8_pipe_redirection.c

/*
    Changes from the previous code:

    - Added the `handlePipe` function to manage command piping.
    - Modified the `executeCommand` function to use the `handlePipe` function.
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 100
#define MAX_ARGS 10

// Helper Functions
void writeMessage(const char *message);
void writeStatusMessage(char *command, int status, long executionTime);

// Read Input
ssize_t readPrompt(char *input, size_t size);

// Process Input
void processUserInput(char *input, ssize_t bytesRead, int *status, long *executionTime);
void executeCommand(char *input, int *status);
void tokenizeInput(char *input, char *args[], size_t *argCount);
void handleRedirection(char *args[], size_t argCount);
void handlePipe(char *args[], size_t argCount);

// Display Status
void displayPromptStatus(int status, long executionTime);



// -------------------- Helper Functions -------------------- //
void writeMessage(const char *message) {
    // Write the message to the standard output
    write(STDOUT_FILENO, message, strlen(message));
}

void writeStatusMessage(char *command, int status, long executionTime) {
    // Create a prompt message with the specified command, status and execution time
    char promptMessage[100];
    snprintf(promptMessage, sizeof(promptMessage), "enseash [%s:%d|%ldms] %% ", command, status, executionTime);
    writeMessage(promptMessage);
}



// --------------------- Read Input --------------------- //
ssize_t readPrompt(char *input, size_t size) {
    // Read input from standard input
    ssize_t bytesRead = read(STDIN_FILENO, input, size);

    // Check for errors during input reading
    if (bytesRead < 0) {
        perror("Error: readPrompt\nread");
        exit(EXIT_FAILURE);
    }

    // Remove trailing newline character (\n)
    input[bytesRead - 1] = '\0';

    // Return the number of bytes read
    return bytesRead;
}



// --------------------- Process Input --------------------- //
void processUserInput(char *input, ssize_t bytesRead, int *status, long *executionTime) {
    // Exit the shell with 'exit' command or Ctrl+D
    if (strcmp(input, "exit") == 0 || bytesRead == 0) {
        if (bytesRead == 0) {
            writeMessage("\n");
        }
        writeMessage("Exiting ENSEA Shell.\n");
        exit(EXIT_SUCCESS);
    }

    // User command
    else {
        // Initialize timestamps (time.h)
        struct timespec start_time, end_time;

        // Get start time
        if (clock_gettime(CLOCK_MONOTONIC, &start_time) != 0) {
            perror("Error: processUserInput (Start Time)\nclock_gettime");
            exit(EXIT_FAILURE);
        }
        
        // Execute the user command and wait for completion
        executeCommand(input, status);

        // Get the end time
        if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0) {
            perror("Error: processUserInput (End Time)\nclock_gettime");
            exit(EXIT_FAILURE);
        }

        // Calculate the execution time in milliseconds
        long seconds = end_time.tv_sec - start_time.tv_sec;
        long nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
        *executionTime = seconds * 1000 + nanoseconds / 1000000;
    }
}

void executeCommand(char *input, int *status) {
    // Create a child process
    pid_t pid = fork();

    // Check for errors
    if (pid == -1) {
        perror("Error: executeCommand\nfork");
        exit(EXIT_FAILURE);
    }

    // Parent process
    else if (pid != 0) {
        // Parent waits for the child process
        wait(status);
    }

    // Child process
    else {
        char *args[MAX_ARGS];
        size_t argCount = 0;

        // Tokenize the input into command and arguments
        tokenizeInput(input, args, &argCount);

        // Handle commands with input and output redirection
        handleRedirection(args, argCount);

        // Handle commands with pipe
        handlePipe(args, argCount);

        // Execute the command using execvp
        execvp(args[0], args);

        // If execvp fails, print an error message
        perror("Error: executeCommand\nexecvp");
        exit(EXIT_FAILURE);
    }
}

void tokenizeInput(char *input, char *args[], size_t *argCount) {
    // Use strtok to split the string into tokens (words) using space as the delimiter
    char *token = strtok(input, " ");
    while (token != NULL) {
        args[(*argCount)++] = token;
        token = strtok(NULL, " ");
    }

    // Set the last element of the args array to NULL as required by execvp
    args[*argCount] = NULL;
}

void handleRedirection(char *args[], size_t argCount) {
    // File for input and output redirection
    char *inputFile = NULL;
    char *outputFile = NULL;

    // Iterate through the arguments to check for input and output redirection
    for (size_t i = 0; i < argCount; i++) {
        // Input redirection
        if (strcmp(args[i], "<") == 0) {
            inputFile = args[i + 1];
            args[i] = NULL; // Remove '<' from the argument list
        }
        
        // Output redirection
        else if (strcmp(args[i], ">") == 0) {
            outputFile = args[i + 1];
            args[i] = NULL; // Remove '>' from the argument list
        }
    }

    // Handle input redirection
    if (inputFile != NULL) {
        // Open the input file for reading
        int fd = open(inputFile, O_RDONLY);
        if (fd == -1) {
            perror("Error: handleRedirection (Input)\nopen");
            exit(EXIT_FAILURE);
        }

        // Redirect standard input to the file
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("Error: handleRedirection (Input)\ndup2");
            close(fd);
            exit(EXIT_FAILURE);
        }
        
        // Close the file descriptor
        close(fd);
    }

    // Handle output redirection
    if (outputFile != NULL) {
        // Open the output file for writing
        int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1) {
            perror("Error: handleRedirection (Output)\nopen");
            exit(EXIT_FAILURE);
        }

        // Redirect standard output to the file
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Error: handleRedirection (Output)\ndup2");
            close(fd);
            exit(EXIT_FAILURE);
        }
        
        // Close the file descriptor
        close(fd);
    }
}

void handlePipe(char *args[], size_t argCount) {
    // Iterate through the arguments to check for pipe redirection
    for (size_t i = 0; i < argCount; i++) {
        if (strcmp(args[i], "|") == 0) {
            // Set the pipe symbol to NULL to separate the first and second command
            args[i] = NULL; 

            // Split the arguments into two parts
            char **firstCommand = &args[0];
            char **secondCommand = &args[i + 1];

            // Check for errors
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("Error: handlePipe\npipe");
                exit(EXIT_FAILURE);
            }

            pid_t childPid = fork();
            if (childPid == -1) {
                perror("Error: handlePipe\nfork");
                exit(EXIT_FAILURE);
            }

            // Parent process: Execute the first command before the pipe
            else if (childPid != 0) {
                // Close the read end of the pipe since the parent writes to it
                close(pipefd[0]);

                // Redirect standard output to the write end of the pipe
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("Error: handlePipe (firstCommand)\ndup2");
                    close(pipefd[1]);
                    exit(EXIT_FAILURE);
                }

                // Close the write end of the pipe as it's no longer needed
                close(pipefd[1]);

                // Execute the first command using execvp
                execvp(firstCommand[0], firstCommand);

                // If execvp fails, print an error message
                perror("Error: handlePipe (firstCommand)\nexecvp");
                exit(EXIT_FAILURE);
            }
            
            // Child process: Execute the second command after the pipe
            else {
                // Close the write end of the pipe since the child reads from it
                close(pipefd[1]);

                // Redirect standard input to the read end of the pipe
                if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                    perror("Error: handlePipe (secondCommand)\ndup2");
                    close(pipefd[0]);
                    exit(EXIT_FAILURE);
                }

                // Close the read end of the pipe as it's no longer needed
                close(pipefd[0]);

                // Execute the second command using execvp
                execvp(secondCommand[0], secondCommand);

                // If execvp fails, print an error message
                perror("Error: handlePipe (secondCommand)\nexecvp");
                exit(EXIT_FAILURE);
            }
        }
    }
}



// --------------------- Display Status --------------------- //
void displayPromptStatus(int status, long executionTime) {
    // Check if the command was successful
    if (WIFEXITED(status)) {
        // If the command exited normally, display exit status in the prompt
        writeStatusMessage("exit", WEXITSTATUS(status), executionTime);
    } else if (WIFSIGNALED(status)) {
        // If the command was terminated by a signal, display signal information in the prompt
        writeStatusMessage("sign", WTERMSIG(status), executionTime);
    }
}



// --------------------- Main --------------------- //
int main() {
    char input[MAX_INPUT_SIZE];
    int status;
    long executionTime;

    // Display the welcome message at launch
    writeMessage("Welcome to ENSEA Shell.\nType 'exit' or press 'Ctrl+D' to quit.\n");

    // Display the shell prompt
    writeMessage("enseash % ");

    // Main loop
    while (1) {
        // Read user input
        ssize_t bytesRead = readPrompt(input, sizeof(input));

        // Process user input and execute the command
        processUserInput(input, bytesRead, &status, &executionTime);

        // Display prompt status
        displayPromptStatus(status, executionTime);
    }

    exit(EXIT_SUCCESS);
}
