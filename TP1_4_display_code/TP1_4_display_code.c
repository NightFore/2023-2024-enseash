// TP1_4_display_code.c

/*
    Changes from the previous code:

    - Added two new functions: writeExitOrSignalMessage and displayPromptStatus.
    - Modified the main loop to call displayPromptStatus after processing user input.
*/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

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

void executeCommand(char *input) {
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

void writeExitOrSignalMessage(char *command, int status) {
    // Create a prompt message with the specified command and status
    char promptMessage[100];
    snprintf(promptMessage, sizeof(promptMessage), "enseash [%s:%d] %% ", command, status);

    // Write the prompt message to the standard output
    writeMessage(promptMessage);
}

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
    } else {
        // Display error prompt
        writeMessage("Error: displayPromptStatus\nenseash % ");
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Display the welcome message at the beginning
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
