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

    // Check for errors during fork
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
        // Get the PID value in a pointer for displayPromptStatus()
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

void executeCommandAndMeasureTime(char *input, ssize_t bytesRead, long *executionTime,pid_t *userCommandPID) {
    // Declare timespec structs (defined in time.h) to store start and end timestamps
    struct timespec start, end;

    // Check if the user wants to exit the shell with 'exit' command or Ctrl+D
    if (strcmp(input, "exit") == 0 || bytesRead == 0) {
        if (bytesRead == 0) {
            writeMessage("\n");
        }
        writeMessage("Exiting ENSEA Shell.\n");
        exit(EXIT_SUCCESS);
    }

    // If the user did not request to exit, execute the entered command and measure its duration with timestamps
    else {
        // Get start time using Linux monotonic time
        if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
            writeMessage("Error: clock_gettime\nenseash %");
        }
        // Execute the user command and store the process ID in userCommandPID
        executeCommand(input,userCommandPID);
        // Get end time using Linux monotonic time
        if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
            writeMessage("Error: clock_gettime\nenseash %");
        }
        // Calculate and store the command duration in milliseconds
        long seconds = end.tv_sec - start.tv_sec;
        long nanoseconds = end.tv_nsec - start.tv_nsec;
        *executionTime = seconds * 1000 + nanoseconds / 1000000;
    }
}

void writeCommandExecutionStatus(char *command, int status,long executionTime) {
    // Create a prompt message with the specified command, status and execution time
    char promptMessage[100];
    snprintf(promptMessage, sizeof(promptMessage), "enseash [%s:%d|%ldms] %% ", command, status,executionTime);

    // Write the prompt message to the standard output
    writeMessage(promptMessage);
}

void displayPromptStatus(long executionTime,pid_t userCommandPID) {
    int status;

    // Wait for the child process from the user command to finish
    // Utilize waitpid() to ensure the correct handling of the desired child process
    waitpid(userCommandPID,&status,0);

    // Check if the command was successful
    if (WIFEXITED(status)) {
        // If the command exited normally, display exit status in the prompt
        writeCommandExecutionStatus("exit", WEXITSTATUS(status),executionTime);
    } else if (WIFSIGNALED(status)) {
        // If the command was terminated by a signal, display signal information in the prompt
        writeCommandExecutionStatus("sign", WTERMSIG(status),executionTime);
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
    writeMessage("Welcome to ENSEA Shell.\nType 'exit' to quit or press Ctrl+D.\n");

    // Display the shell prompt
    writeMessage("enseash % ");

    // Main loop
    while (1) {
        // Read user input
        ssize_t bytesRead = readPrompt(input, sizeof(input));

        // Process user input and time measurement
        executeCommandAndMeasureTime(input, bytesRead,&executionTime,&userCommandPID);

        // Display prompt status and execution time
        displayPromptStatus(executionTime,userCommandPID);
    }

    exit(EXIT_SUCCESS);
}
