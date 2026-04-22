/*
NAME        : V.V.DURGADEVI
DATE        : 6 APRIL 2026
DESCRIPTION : This project implements a simple Linux command-line shell called MINI SHELL,
              which allows users to enter and execute commands just like Bash. 
              It supports features such as command execution, built-in commands, signal handling, background processes, and piping
*/
#include "header.h"
// default shell prompt
char prompt[20] = "Minishell:$";
// buffer to store user input
char input_string[100];
// array to store external commands (from file)
char *external_commands[200];
char buf[200];
// list of built-in commands
char *builtins[] = {
    "echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs",
    "let", "eval", "set", "unset", "export", "declare", "typeset",
    "readonly", "getopts", "source", "exit", "exec", "shopt",
    "caller", "true", "type", "hash", "bind", "help","clear", NULL};
// global status variable to store exit status of last command
int status = 0;
int main(int argc, char *argv[])
{
    getcwd(buf, sizeof(buf)); // getting the current working directly path and storing it in a buf_cwd
    // ===== SIGNAL REGISTRATION =====
    // register own signal handler for Ctrl+C
    signal(SIGINT, signal_handler);
    // register own signal handler for Ctrl+Z
    signal(SIGTSTP, signal_handler);
    // register handler for background process completion
    signal(SIGCHLD, signal_handler);
    
    //signal(SIGABRT, error_handler);
    // clear the screen
    system("clear");
    // ===== LOAD EXTERNAL COMMANDS =====
    // read commands from file and store in array
    extract_external_commands(external_commands);
    // ===== START SHELL LOOP =====
    // continuously take input and execute commands
    scan_input(prompt, input_string);
    return 0;
}