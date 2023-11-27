#include <unistd.h>
#include <stdlib.h>

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

    // Return input
    return bytesRead;
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Display the welcome message at the beginning
    displayWelcomeMessage();

    while (1) {
        // Display the shell prompt and read user input
        displayPrompt();
        readPrompt(input, sizeof(input));
    }

    exit(EXIT_SUCCESS);
}
