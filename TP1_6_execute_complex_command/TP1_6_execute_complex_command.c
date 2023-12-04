// TP1_6_execute_complex_command.c

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 100
#define MAX_ARGS 10

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
        char *args[MAX_ARGS];
        size_t argCount = 0;

        // Tokenize the input into command and arguments
        tokenizeInput(input, args, &argCount);

        // Execute the command using execvp
        execvp(args[0], args);

        // If execvp fails, print an error message
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
    writeMessage("Welcome to ENSEA Shell.\nType 'exit' to quit or press Ctrl+D.\n");

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
