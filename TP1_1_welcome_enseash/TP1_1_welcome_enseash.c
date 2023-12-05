// TP1_1_welcome_enseash.c

#include <unistd.h>

// Helper Functions
void displayWelcomeMessage();



// -------------------- Helper Functions -------------------- //
void displayWelcomeMessage() {
    const char welcomeMessage[] = "Welcome to ENSEA Shell.\n"
                                  "Type 'exit' to quit.\n"
                                  "enseash %\n";

    // Write to the standard output (file descriptor, message pointer, message size)
    write(STDOUT_FILENO, welcomeMessage, sizeof(welcomeMessage) - 1);
}



// --------------------- Main --------------------- //
int main() {
    displayWelcomeMessage();

    return 0;
}
