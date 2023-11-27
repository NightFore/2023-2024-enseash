#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_INPUT_SIZE 100

void displayWelcomeMessage() {
    const char welcomeMessage[] = "Welcome to ENSEA Tiny Shell.\n"
                                  "Type 'exit' to quit.\n";

    // Write the welcome message to the standard output
    write(STDOUT_FILENO, welcomeMessage, sizeof(welcomeMessage) - 1);
}

void displayPrompt() {
    const char prompt[] = "enseash % ";

    // Write the shell prompt to the standard output
    write(STDOUT_FILENO, prompt, sizeof(prompt) - 1);
}

ssize_t readPrompt(char *input, size_t size) {
    // Read input from standard input
    ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input));

    // Check for errors
    if (bytesRead < 0) {
        const char errorInputMessage[] = "Error: readPrompt.\n";
        write(STDOUT_FILENO, errorInputMessage, sizeof(errorInputMessage) - 1);
        exit(EXIT_FAILURE);
    }

    // Remove trailing newline character (\n)
    input[bytesRead - 1] = '\0';

    // Return input
    return bytesRead;
}

void helloWorld() {
    const char helloMessage[] = "-----Hello World-----\n";

    // Write the hello message to the standard output
    write(STDOUT_FILENO, helloMessage, sizeof(helloMessage) - 1);
}

void executeCommand(char *input) {
    pid_t pid = fork();

    // Check for errors
    if (pid == 0) {
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
        printf("zeaea %d\n", getpid());
        execl("/bin/ls", "ls", "-l", "*.c", (char *) NULL);
        
        // The code below should not be executed if execl is successful
        const char errorExecMessage[] = "Error: executeCommand.\n"
                                        "This line must not be printed";
        write(STDOUT_FILENO, errorExecMessage, sizeof(errorExecMessage) - 1);
        exit(EXIT_FAILURE);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Display the welcome message at the beginning
    displayWelcomeMessage();

    while (1) {
        // Display the shell prompt and read user input
        displayPrompt();
        ssize_t bytesRead = readPrompt(input, sizeof(input));
        
        // Check if the user entered the "hello" command
        if (strcmp(input, "hello") == 0) {
            helloWorld();
        }
        else {
            executeCommand(input);
        }
    }

    exit(EXIT_SUCCESS);
}
