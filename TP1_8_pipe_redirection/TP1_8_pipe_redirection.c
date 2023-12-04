// TP1_8_pipe_redirection.c

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 100
#define MAX_ARGS 10

// Helper Functions
void writeMessage(const char *message);
void writeExitOrSignalMessage(char *command, int status);

// Read Input
ssize_t readPrompt(char *input, size_t size);

// Process Input
void processUserInput(char *input, ssize_t bytesRead);
void executeCommand(char *input);
void tokenizeInput(char *input, char *args[], size_t *argCount);
void handleRedirection(char *args[], size_t argCount);
void handlePipe(char *args[], size_t argCount);

// Display Status
void displayPromptStatus();



// -------------------- Helper Functions -------------------- //
void writeMessage(const char *message) {
    // Write the message to the standard output
    write(STDOUT_FILENO, message, strlen(message));
}

void writeExitOrSignalMessage(char *command, int status) {
    // Create a prompt message with the specified command and status
    char promptMessage[100];
    snprintf(promptMessage, sizeof(promptMessage), "enseash [%s:%d] %% ", command, status);
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
void processUserInput(char *input, ssize_t bytesRead) {
    // Exit the shell with 'exit' command or Ctrl+D
    if (strcmp(input, "exit") == 0 || bytesRead == 0) {
        if (bytesRead == 0) {
            writeMessage("\n");
        }
        writeMessage("Exiting ENSEA Shell.\n");
        exit(EXIT_SUCCESS);
    }

    // Execute the user command
    else {
        executeCommand(input);
    }
}

void executeCommand(char *input) {
    // Create a child process
    pid_t pid = fork();

    // Check for errors
    if (pid == -1) {
        perror("Error: executeCommand\nfork");
        exit(EXIT_FAILURE);
    }

    // Parent process
    else if (pid != 0) {
        int status;
        wait(&status);
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



// --------------------- Display status --------------------- //
void displayPromptStatus() {
    int status;

    // Wait for the child process to finish
    wait(&status);

    // Check if the command was successful
    if (WIFEXITED(status)) {
        // Display exit status in the prompt
        writeExitOrSignalMessage("exit", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        // Display signal information in the prompt
        writeExitOrSignalMessage("sign", WTERMSIG(status));
    }
}



// --------------------- Main --------------------- //
int main() {
    char input[MAX_INPUT_SIZE];

    // Display the welcome message at launch
    writeMessage("Welcome to ENSEA Shell.\nType 'exit' or press 'Ctrl+D' to quit.\n");

    // Display the shell prompt
    writeMessage("enseash % ");

    // Main loop
    while (1) {
        // Read user input
        ssize_t bytesRead = readPrompt(input, sizeof(input));

        // Process user input
        processUserInput(input, bytesRead);

        // Display prompt status
        displayPromptStatus();
    }

    exit(EXIT_SUCCESS);
}
