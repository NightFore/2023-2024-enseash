// TP1_6_execute_complex_command.c

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
