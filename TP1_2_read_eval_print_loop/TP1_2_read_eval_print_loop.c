// TP1_2_read_eval_print_loop.c

/*
    Changes from the previous code:

    - Added header files (stdlib.h, string.h, stdio.h, sys/wait.h).
    - Defined a constant MAX_INPUT_SIZE to specify the maximum size for user input.
    - Created new functions: writeMessage, readPrompt, and executeCommand.
    - In the main function:
        - Initialized a character array `input` to store user input.
        - Used writeMessage to display the welcome message.
        - Implemented a main loop for the shell to continuously read user input and execute commands.
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

        // Execute the user command
        executeCommand(input);
    }

    exit(EXIT_SUCCESS);
}
