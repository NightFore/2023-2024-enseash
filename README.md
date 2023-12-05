# ENSEA in the Shell

## Overview

"ENSEA in the Shell" is a simple shell program developed as part of the ENSEA Computer Science course. The shell offers a basic command-line interface, allowing users to input commands while supporting features like input or output redirection, piping and execution time tracking.

## Features

- **Command Execution:** The shell can execute user-entered commands.
- **Redirection:** Supports input and output redirection using `<` and `>` operators.
- **Piping:** Handles commands separated by the `|` symbol, enabling multiple commands.
- **Execution Time Tracking:** Measures and displays the execution time of each command.

## Getting Started

### Prerequisites

- The program is written in C and requires a C compiler (e.g., GCC) to build.

### Building the Shell

```bash
gcc program_name.c -o enseash
```
*Replace "program_name.c" with the specific code file from any question's folder.

### Running the Shell

```bash
./enseash
```

## Usage

1. **Welcome Message:**
   - Upon launching the shell, a welcome message is displayed.

2. **Entering Commands:**
   - Users can enter commands at the shell prompt (`enseash %`).
   - To exit the shell, type `exit` or press `Ctrl+D`.

3. **Execution Time:**
   - The shell tracks and displays the execution time of each command in milliseconds.

4. **Features:**
   - **Redirection:**
     - Use `<` for input redirection: `command < input.txt`
     - Use `>` for output redirection: `command > output.txt`
   - **Piping:**
     - Separate commands with `|` for piping: `command1 | command2`

## Code Structure

- **Header Files:**
  - `<fcntl.h>`, `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<sys/wait.h>`, `<time.h>`, `<unistd.h>`: Standard C libraries.
  
- **Constants:**
  - `MAX_INPUT_SIZE`: Maximum size for user input (default value: 100).
  - `MAX_ARGS`: Maximum number of command arguments (default value: 10).

- **Helper Functions:**
  - `writeMessage(const char *message)`: Writes a message to the standard output.
  - `writeStatusMessage(char *command, int status, long executionTime)`: Generates and displays a prompt message with command, status, and execution time.

- **Input Handling:**
  - `readPrompt(char *input, size_t size)`: Reads user input from standard input.
  - `processUserInput(char *input, ssize_t bytesRead, int *status, long *executionTime)`: Processes user input, executes commands, and tracks execution time, with the resulting command execution status stored in the `status` variable.

- **Command Execution:**
  - `executeCommand(char *input, int *status)`: Creates a child process to execute the command using `execvp`.
  - `tokenizeInput(char *input, char *args[], size_t *argCount)`: Tokenizes user input into command and arguments.
  - `handleRedirection(char *args[], size_t argCount)`: Handles input and output redirection.
  - `handlePipe(char *args[], size_t argCount)`: Handles command piping.

- **Status Display:**
  - `displayPromptStatus(int status, long executionTime)`: Displays the prompt status based on command completion.

- **Main Function:**
  - The main function initializes variables, displays a welcome message, and enters a loop to read user input, process commands, and display prompt status.

## Results

1. **Simple Display using the `write()` Function**
   - Upon executing the shell, users are welcomed with the following message:
   ```
   Welcome to ENSEA Shell.
   Type 'exit' to quit.
   enseash %
   ```

2. **Read–Eval–Print Loop (REPL) Environment:**
   - The shell supports a basic REPL environment. Users can enter commands (`ls`, `date`, `fortune`, ...), and the shell promptly executes them.
   ```
   enseash % ls
   TP1_2_read_eval_print_loop    TP1_2_read_eval_print_loop.c
   enseash % fortune
   You will be awarded the Nobel Peace Prize... posthumously
   enseash %
   ```
   * Note: The `fortune` command may not be available by default on all systems.

3. **Exit Commands:**
   - To exit the shell, users can now enter the command `exit` or use `Ctrl+D`, and the shell will display a corresponding exit message.
   ```
   Welcome to ENSEA Shell.
   Type 'exit' or press 'Ctrl+D' to quit.
   enseash % exit
   Exiting ENSEA Shell.
   ```

4. **Prompt Status:**
   - The shell displays the return code/signal of the previous command in the prompt.
   ```
   enseash % ls
   enseash [exit:0] % something
   Error: executeCommand
   execvp: No such file or directory
   enseash [exit:1] %
   ```
   * Note: The numerical values 0 and 1 correspond respectively to EXIT_SUCCESS and EXIT_FAILURE.

5. **Execution Time Tracking:**
   - The `processUserInput` function timestamps the execution command call using `clock_gettime`. With each timestamp, it captures and stores the current time in `struct timespec` variables, encompassing both seconds and nanoseconds components. The duration is then calculated in milliseconds and displayed in `writeStatusMessage`.
   ```
   enseash % pwd
   /TP1_5_measure_execution_time
   enseash [exit:0|2ms] %
   ```

6. **Command Arguments:**
   - The shell now supports command arguments. It utilizes `strtok` for parsing and `execvp` for effective command execution with specified arguments.
   ```
   enseash % ls -l directory
   total 0
   enseash [exit:0|3ms] %
   ```

7. **Redirection:**
   - The shell manages input and output redirection using `<` and `>` symbols. The `handleRedirection` function analyzes command-line arguments for these symbols, enabling proper redirection.
   ```
   enseash % ls > text.txt
   enseash [exit:0|3ms] % wc -l < text.txt
         3
   enseash [exit:0|5ms] % 
   ```

8. **Pipe Redirection:**
   - The `handlePipe` function identifies the `|` symbol in command-line arguments, separates commands at the pipe, enabling the execution of multiple commands in succession. It allows for the execution of the second command after the pipe.
   ```
   enseash % ls | wc -l
         2
   enseash [exit:0|6ms] % 
   ```

## Contributing

This project is part of an academic assignment and is not open to external contributions.

## License

This project is licensed under the [MIT License](LICENSE).
