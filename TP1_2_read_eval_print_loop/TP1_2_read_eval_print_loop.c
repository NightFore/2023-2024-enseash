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
    ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input));

    // Check for errors during input reading
    if (bytesRead < 0) {
        writeMessage("Error: readPrompt.\n");
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
        // Execute the command using execl
        // The "/bin/sh" is the path to the system shell
        // The "sh" is the name of the shell
        // The "-c" indicates that the next argument is a command string
        // The input is the command string to be executed
        execl("/bin/sh", "sh", "-c", input, (char *)NULL);

        // If execl fails, print an error message
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
        executeCommand(input);
    }

    exit(EXIT_SUCCESS);
}