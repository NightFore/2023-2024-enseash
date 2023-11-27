#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 100

void displayWelcomeMessage() {
    const char welcomeMessage[] = "Welcome to ENSEA Tiny Shell.\n"
                                  "Type 'exit' to quit.\n"
                                  "enseash %";
    write(STDOUT_FILENO, welcomeMessage, sizeof(welcomeMessage) - 1);
}

int main() {
    char input[MAX_INPUT_SIZE];

    displayWelcomeMessage();

    return 0;
}
