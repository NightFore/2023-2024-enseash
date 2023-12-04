// TP1_4_display_code.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 100

// Helper Functions
void writeMessage(const char *message);
void writeStatusMessage(char *command, int status);

// Read Input
ssize_t readPrompt(char *input, size_t size);

// Process Input
void processUserInput(char *input, ssize_t bytesRead, int *status);
void executeCommand(char *input, int *status);

// Display Status
void displayPromptStatus(int status);



// -------------------- Helper Functions -------------------- //
void writeMessage(const char *message) {
    // Write the message to the standard output
    write(STDOUT_FILENO, message, strlen(message));
}

void writeStatusMessage(char *command, int status) {
    // Create a prompt message with the specified command, status and execution time
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
void processUserInput(char *input, ssize_t bytesRead, int *status) {
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
        executeCommand(input, status);
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
        // Execute the command (without arguments)
        execlp(input, input, (char*) NULL);

        // If execlp fails, print an error message
        perror("Error: executeCommand\nexecvp");
        exit(EXIT_FAILURE);
    }
}



// --------------------- Display Status --------------------- //
void displayPromptStatus(int status) {
    // Check if the command was successful
    if (WIFEXITED(status)) {
        // If the command exited normally, display exit status in the prompt
        writeStatusMessage("exit", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        // If the command was terminated by a signal, display signal information in the prompt
        writeStatusMessage("sign", WTERMSIG(status));
    }
}



// --------------------- Main --------------------- //
int main() {
    char input[MAX_INPUT_SIZE];
    int status;

    // Display the welcome message at launch
    writeMessage("Welcome to ENSEA Shell.\nType 'exit' or press 'Ctrl+D' to quit.\n");

    // Display the shell prompt
    writeMessage("enseash % ");

    // Main loop
    while (1) {
        // Read user input
        ssize_t bytesRead = readPrompt(input, sizeof(input));

        // Process user input and execute the command
        processUserInput(input, bytesRead, &status);

        // Display prompt status
        displayPromptStatus(status);
    }

    exit(EXIT_SUCCESS);
}
