# Synthesis Practical Work 1 - Ensea in the Shell

## Objectives
This project involves the creation of a tiny shell in C language, displaying exit codes and execution times of launched programs

## Functionalities
- Display a welcome message followed by a simple prompt
- Execution of the entered command and return to the prompt
- Management of the shell output with the command `exit` or with `Ctrl+d`
- Display the return code/signal and execution time of the previous command in the prompt
- Execution of complex commands (with arguments)
- Management of redirections to `stdin` and `stdout` with `<` and `>`
- Management of pipe redirection with `|`
- Return to the prompt immediatly with `&` (execution of programs in the background)

## Project Structure
Each file contains the .c file and the executing program of the corresponding question. Starting from question 2, the .c files contains a description of updates.

## Operating Instructions
1. Clone this repository on your machine
2. Place yourself in the right directory
3. If the executing program is missing or not compatible :
   - Compile the program (with gcc for example) :
   ```
   gcc program_name.c -o executing_program_name
   ```
   - Execute program :
   ```
   ./executing_program_name
   ```

## Results
1. Employ the write() function for a straightforward display
```
Welcome to ENSEA Tiny Shell.
Type 'exit' to quit.
enseash %
```
2. The primary functionality of our tiny shell involves the execution of commands entered by the user, creating a basic Read–Eval–Print Loop (REPL) environment. The following steps outline the process :
   1. **Read Command from User** : The `read` function is employed to capture the command entered by the user.
   2. **Command Execution** : Creating a child process using `fork`, followed by execution and error handling through `execlp`.
   3. **Print Prompt and Await New Command** : Call these functions in an infinite loop.

As an example with commands `ls` and `fortune` : 
```
enseah % ls
TP1_2_read_eval_print_loop    TP1_2_read_eval_print_loop.c
enseah % fortune
You will be awarded the Nobel Peace Prize... posthumously
enseah %
```
3. The `exit` and `Ctrl+d` commands have been explicitly defined to exit the tiny shell. These commands are managed alongside the command execution handling in a `processUserInput` function.
```
enseash % exit
Exiting ENSEA Shell.
$
```
4. In the context of the `wait` function, the macro `WEXITSTATUS` yields the output status of the preceding command's child process in the prompt, whereas `WTERMSIG` provides the signal that terminated it. A `writeExitOrSignalMessage` function is defined to display the output.
```
enseash [exit:value] %
enseash [sign:value] %
```
5. In our `processUserInput` function, we manage execution time by timestamping the execution command call using `clock_gettime`. With each timestamp, we capture and store the current time in `struct timespec` variables, encompassing both seconds and nanoseconds components. The duration is then calculated in milliseconds and displayed in writeExitOrSignalMessage.
```
enseash [exit:value|duration ms] %
enseash [sign:value|duration ms] %
```
6. We aim to incorporate command arguments in our implementation. To achieve this, we use `strtok` for parsing the character input into distinct words, and in the `executeCommand` function, we replace `execlp` with `execvp` to effectively manage command arguments during user command execution.


7. In the `handleRedirection` function, input and output redirection using `<` and `>` symbols is managed. The function analyzes command-line arguments for these symbols, indicating requests for input and output redirection. It employs `dup2` to redirect `stdin` or `stdout` to the designated files while ensuring correct file access permissions for reading or writing are maintained throughout the redirection.


8. The `handlePipe` function identifies a pipe symbol `|` in command-line arguments, separates commands at the pipe, and sets up a communication pipe. It then forks a child process to execute the second command after the pipe, with proper redirection of input and output. The parent and child processes each execute their respective commands, handling errors with informative messages.