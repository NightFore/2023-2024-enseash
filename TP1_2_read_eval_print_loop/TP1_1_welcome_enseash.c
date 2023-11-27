#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 100

void displayWelcomeMessage() {
    const char welcomeMessage[] = "Welcome to ENSEA Tiny Shell.\n"
                                  "Type 'exit' to quit.\n";
    write(STDOUT_FILENO, welcomeMessage, sizeof(welcomeMessage) - 1);
}

void displayPrompt() {
    const char prompt[] = "enseash % ";
    write(STDOUT_FILENO, prompt, sizeof(prompt) - 1);
}

ssize_t readPrompt(char *input, size_t size) {
    ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input));

    if (bytesRead <= 0) {
        write(STDOUT_FILENO, "Error reading input.\n", 21);
        exit(EXIT_FAILURE);
    }

    // Remove trailing newline character
    input[bytesRead - 1] = '\0';

    return bytesRead;
}

int main() {
    char input[MAX_INPUT_SIZE];

    displayWelcomeMessage();

    while (1) {
        displayPrompt();
        ssize_t bytesRead = readPrompt(input, sizeof(input));

        if (strcmp(input, "exit") == 0) {
            write(STDOUT_FILENO, "Exiting ENSEA Tiny Shell. Goodbye!\n", 34);
            break;
        } else {
            write(STDOUT_FILENO, "Executing command: ", 19);
            write(STDOUT_FILENO, input, bytesRead - 1);
            write(STDOUT_FILENO, "\n", 1);
        }
    }

    return 0;
}
