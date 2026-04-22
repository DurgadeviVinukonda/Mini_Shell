/*
NAME        : V.V.DURGADEVI
DATE        : 6 APRIL 2026
DESCRIPTION : This project implements a simple Linux command-line shell called MINI SHELL,
              which allows users to enter and execute commands just like Bash. 
              It supports features such as command execution, built-in commands, signal handling, background processes, and piping
*/
#include "header.h"
pid_t c1;           // head pointer of job linked list
Slist *head = NULL; // head pointer of job linked list
// function to take input and process commands
void scan_input(char *prompt, char *input_string)
{
    printf("--------------------------------------\n");
    printf(ANSI_COLOR_MAGENTA"\t\tMINISHELL\t\t\n"ANSI_COLOR_RESET);
    printf("--------------------------------------\n");
    while (1) // infinite loop for shell
    {
        printf(ANSI_COLOR_GREEN"%s "ANSI_COLOR_RESET, prompt); // display prompt
        // clear stdin buffer
        fflush(stdin);
        input_string[0] = '\0'; // clear previous input
        // read full line input (including spaces)
        scanf("%[^\n]", input_string);
        getchar();
        // if user presses only Enter, skip
        if (input_string[0] == '\0') // if given just \n, again continuing
        {
            continue;
        }
        // ===== PS1 (prompt change) =====
        if (strncmp(input_string, "PS1", 3) == 0)
        {
            // check if no spaces in command
            if ((strchr(input_string, 32) == NULL) && (strchr(input_string, '=') != NULL) && input_string[4] != '\0' )
            {
                // copy only after "PS1="
                strcpy(prompt, input_string + 4);
            }
            else
            {
                printf(ANSI_COLOR_RED"PS1: command not found\n"ANSI_COLOR_RESET);
            }
            continue;
        }
        // jobs commands
        if (strcmp(input_string, "jobs") == 0)
        {
            print_list(head); // print all jobs
            continue;
        }
        // fg command
        else if (strcmp(input_string, "fg") == 0)
        {
            fg(&head); // resume last job to foreground
            continue;
        }
        // bg command
        else if (strcmp(input_string, "bg") == 0)
        {
            bg(&head); // resume last job in background
            continue;
        }
        // copy input for parsing
        char temp[1024];
        strcpy(temp, input_string);
        // extract command (first word)
        char *command = get_command(temp);
        // check cmd_type(built-in / external )
        int check_cmd;
        check_cmd = check_command_type(command);
        if (check_cmd == BUILTIN)                    // built-in
            execute_internal_commands(input_string); // execute internal commands
        else if (check_cmd == EXTERNAL)              // external commands
        {
            // create the child process
            c1 = fork();
            if (c1 == 0) // child process
            {
                // reset signals to default for child
                signal(SIGINT, SIG_DFL);                 // allow Ctrl+C in child
                signal(SIGTSTP, SIG_DFL);                // allow Ctrl+Z in child
                execute_external_commands(input_string); // execute external commands
                exit(0);
            }
            else if (c1 > 0)                     // parent process
                waitpid(c1, &status, WUNTRACED); // wait for child to finish or stop (Ctrl+Z)
            else
                perror("fork failed");
        }
        else
            printf(ANSI_COLOR_RED"Command not found\n"ANSI_COLOR_RESET);
    }
}