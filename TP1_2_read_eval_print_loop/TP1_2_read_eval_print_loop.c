#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_INPUT_SIZE 100

void writeMessage(const char *message) {
    // Write the message to the standard output
    write(STDOUT_FILENO, message, strlen(message));
}

ssize_t readPrompt(char *input, size_t size) {
    // Read input from standard input
    ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input));

    // Check for errors during input reading
    if (bytesRead < 0) {
        writeMessage("Error: readPrompt.\n");
        exit(EXIT_FAILURE);
    }

    // Remove trailing newline character (\n)
    input[bytesRead - 1] = '\0';

    // Return input
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

    // Parent code
    else if (pid != 0) {
        int status;
        wait(&status);
    }

    // Child code
    else {
        // The code below should not be executed if execl is successful
        writeMessage("Error: executeCommand - This line must not be printed.\n");
        exit(EXIT_FAILURE);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Display the welcome message at the beginning
    writeMessage("Welcome to ENSEA Tiny Shell.\nType 'exit' to quit.\n");

    // Main loop
    while (1) {
        // Display the shell prompt
        writeMessage("enseash % ");

        // Read user input
        ssize_t bytesRead = readPrompt(input, sizeof(input));

        // Check the user input for specific commands
        if (strcmp(input, "exit") == 0) {
            // Exit shell if user entered "exit"
            break;
        }
        else if (strcmp(input, "hello") == 0) {
            // Check if the user entered the "hello" command
            writeMessage("-----Hello World-----\n");
        }
        else {
            // Execute the user command
            executeCommand(input);
        }
    }

    exit(EXIT_SUCCESS);
}
