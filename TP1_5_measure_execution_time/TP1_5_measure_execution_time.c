// TP1_5_measure_execution_time.c

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_INPUT_SIZE 100

void writeMessage(const char *message) {
    // Write the message to the standard output
    write(STDOUT_FILENO, message, strlen(message));
}

ssize_t readPrompt(char *input, size_t size) {
    // Read input from standard input
    ssize_t bytesRead = read(STDIN_FILENO, input, size);

    // Check for errors during input reading
    if (bytesRead < 0) {
        writeMessage("Error: readPrompt\n");
        exit(EXIT_FAILURE);
    }

    // Remove trailing newline character (\n)
    input[bytesRead - 1] = '\0';

    // Return the number of bytes read
    return bytesRead;
}

void executeCommand(char *input,pid_t *childPID) {
    // Create a child process
    pid_t pid = fork();

    // Check for errors
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Parent process code
    else if (pid != 0) {
        int status;
        wait(&status);
    }

    // Child process code
    else {
        // Get the PID value
        *childPID = getpid();
        // Execute the command using execlp:
        // - Path to the executable
        // - Program name
        // - (char*) NULL marks the end of the argument list
        execlp(input, input, (char*) NULL);

        // If execl fails, print an error message
        writeMessage("Error: executeCommand - This line must not be printed.\n");
        exit(EXIT_FAILURE);
    }

}

void processUserInputAndExecutionTimeMeasurement(char *input, ssize_t bytesRead, long *executionTime,pid_t *userCommandPID) {
    struct timespec start, end;

    // Exit the shell with 'exit' command or Ctrl+D
    if (strcmp(input, "exit") == 0 || bytesRead == 0) {
        if (bytesRead == 0) {
            writeMessage("\n");
        }
        writeMessage("Exiting ENSEA Shell.\n");
        exit(EXIT_SUCCESS);
    }

    // Execute the user command and measure its duration by timestamping
    else {
        // Get start time
        if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
            writeMessage("Error: clock_gettime\nenseash %");
        }
        // Need to get the user command PID
        executeCommand(input,userCommandPID);
        // Get end time
        if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
            writeMessage("Error: clock_gettime\nenseash %");
        }
        // Calculate duration in milliseconds
        long seconds = end.tv_sec - start.tv_sec;
        long nanoseconds = end.tv_nsec - start.tv_nsec;
        *executionTime = seconds * 1000 + nanoseconds / 1000000;
    }
}

void writeExitOrSignalMessage(char *command, int status,long executionTime) {
    // Create a prompt message with the specified command and status
    char promptMessage[100];
    snprintf(promptMessage, sizeof(promptMessage), "enseash [%s:%d|%ldms] %% ", command, status,executionTime);

    // Write the prompt message to the standard output
    writeMessage(promptMessage);
}

void displayPromptStatus(long executionTime,pid_t userCommandPID) {
    int status;

    // Wait for the child process from the user command to finish
//    wait(&status);
    waitpid(userCommandPID,&status,0);

    // Check if the command was successful
    if (WIFEXITED(status)) {
        // Display exit status in the prompt
        writeExitOrSignalMessage("exit", WEXITSTATUS(status),executionTime);
    } else if (WIFSIGNALED(status)) {
        // Display signal information in the prompt
        writeExitOrSignalMessage("sign", WTERMSIG(status),executionTime);
    } else {
        // Display error prompt
        writeMessage("Error: displayPromptStatus\nenseash % ");
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    long executionTime;
    pid_t userCommandPID;

    // Display the welcome message at the beginning
    writeMessage("Welcome to ENSEA Shell.\nType 'exit' or press 'Ctrl+D' to quit.\n");

    // Display the shell prompt
    writeMessage("enseash % ");

    // Main loop
    while (1) {
        // Read user input
        ssize_t bytesRead = readPrompt(input, sizeof(input));

        // Process user input and time measurement
        processUserInputAndExecutionTimeMeasurement(input, bytesRead,&executionTime,&userCommandPID);

        // Display prompt status and execution time
        displayPromptStatus(executionTime,userCommandPID);
    }

    exit(EXIT_SUCCESS);
}
