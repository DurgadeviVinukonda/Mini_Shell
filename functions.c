/*
NAME        : V.V.DURGADEVI
DATE        : 6 APRIL 2026
DESCRIPTION : This project implements a simple Linux command-line shell called MINI SHELL,
              which allows users to enter and execute commands just like Bash.
              It supports features such as command execution, built-in commands, signal handling, background processes, and piping
*/
#include "header.h"
extern char buf[200];
/*=======INTERNAL COMMANDS=====*/
void execute_internal_commands(char *input_string)
{
    char temp[100];
    strcpy(temp, input_string);    // copy input because strtok modifies string
    char *cmd = strtok(temp, " "); // extract command (first word)

    // exit command
    if (strcmp(cmd, "exit") == 0)
    {
        exit(0); // terminate shell
    }
    // pwd Command
    else if (strcmp(cmd, "pwd") == 0)
    {
        char buf_cwd[100];
        getcwd(buf_cwd, sizeof(buf_cwd)); // getting the current working directly path and storing it in a buf_cwd

        printf(ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, buf_cwd); // print current directory
    }
    // cd Command
    // else if (strcmp(cmd, "cd") == 0)
    // {
    //     chdir(temp + 3); // change directory (skip "cd ")
    // }
    else if (strcmp(cmd, "cd") == 0)
    {
        char *dir = strtok(NULL, " \t\n"); // get next argument

        if (dir == NULL)
        {
            // printf("cd: missing argument\n");
        }
        else
        {
            if (chdir(dir) != 0)
            {
                perror("cd failed");
            }
        }
    }
    // clear command
    else if (strcmp(cmd, "clear") == 0)
    {
        system("clear");
    }
    // echo Command
    else if (strstr(input_string, "echo"))
    {
        // echo $$ (print process id)
        if (strcmp(input_string, "echo $$") == 0)
        {
            // prints the pid
            printf("%d\n", getpid());
        }
        // echo $? print last command status
        else if (strcmp(input_string, "echo $?") == 0)
        {
            // prints the exit status of previous executed command
            if (WIFEXITED(status))
                printf("Terminated normally with exit status -> %d\n", WEXITSTATUS(status));
            else
                printf("Terminated abnormally with exit code %d\n", status);
        }
        // echo $SHELL print current directory
        else if (strcmp(input_string, "echo $SHELL") == 0)
        {
            printf(ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, buf); // printing it
        }
        else
        { // else echo will act as printf, whatever passed after echo should print in the stdout
            char buf[100];
            char *space = strchr(input_string, ' '); // printing the content after the first space
            strcpy(buf, space + 1);
            printf("%s\n", buf);
        }
    }
}
/*========EXTERNAL COMMANDS========*/
void execute_external_commands(char *input_string)
{
    char temp[100];
    strcpy(temp, input_string); // copy input

    // converting the commands given by user into 2d array
    char *args[100];
    int i = 0, pipe_flag = 0;

    // we used strtok and making tokens using space as delimeter
    char *token = strtok(temp, " ");
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // last element must be NULL for execvp
    int argc = i;
    i = 0;
    // check if pipe symbol "|" is present
    while (args[i])
    {
        if (strcmp(args[i], "|") == 0)
        {
            pipe_flag = 1;
            break;
        }
        i++;
    }
    // if pipe passed calling pipe function
    if (pipe_flag)
    {
        npipe(argc, args);
        exit(0);
    }
    // else calling execvp to execute the command
    else
    {
        execvp(args[0], args);
        perror("execvp"); // if execvp fails
        exit(1);
    }
}
int npipe(int argc, char *argv[])
{
    // array to store the command indexes
    int arr[argc];
    // int *arr = malloc(argc * sizeof(int));
    int j = 0, count = 1;
    // cmd index starting from 0
    arr[j] = 0;
    j++;
    // getting command indexes
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            count++;        // increase command count
            arr[j] = i + 1; // next command starts after pipe
            j++;
            argv[i] = NULL; // split command
        }
    }
    // creating the child process
    int restore = dup(0); // save original stdin BEFORE loop
    for (int i = 0; i < count; i++)
    {
        int fd[2];
        // create the pipe execpt for last command
        if (i < count - 1)
        {
            if (pipe(fd) == -1)
            {
                perror("pipe");
                return -1;
            }
        }
        pid_t c1 = fork();
        if (c1 > 0) // parent process
        {
            if (i < count - 1)
            {
                close(fd[1]);   // close write end
                dup2(fd[0], 0); // read input from pipe
                close(fd[0]);
            }
        }
        if (c1 == 0) // child process
        {
            if (i < count - 1)
            {
                close(fd[0]);   // close read end
                dup2(fd[1], 1); // write output to pipe
                close(fd[1]);
            }
            // executing the commands
            execvp(argv[arr[i]], argv + arr[i]);
        }
    }
    // wait for all children
    dup2(restore, 0); // restoring the stdin so we can give the next command
    close(restore);
    // wait for all child processes
    for (int i = 0; i < count; i++)
        wait(NULL);
    return 0;
}
int check_command_type(char *command)
{
    // check built-in commands
    for (int i = 0; builtins[i] != NULL; i++)
    {
        if (strcmp(command, builtins[i]) == 0)
        {
            return BUILTIN;
        }
    }
    // check external commands
    for (int i = 0; external_commands[i] != NULL; i++)
    {
        if (strcmp(command, external_commands[i]) == 0)
        {
            return EXTERNAL;
        }
    }
    return NO_COMMAND; // mot found
}
char *get_command(char *input_string)
{
    static char cmd[100];
    int i = 0;
    // extract command till space
    while (input_string[i] != ' ' && input_string[i] != '\0')
    {
        cmd[i] = input_string[i];
        i++;
    }
    cmd[i] = '\0'; // terminate the string
    return cmd;
}
void extract_external_commands(char **external_commands)
{
    int fd = open("External_command.txt", O_RDONLY); // file containing commands
    if (fd < 0)
    {
        perror("open");
        return;
    }
    char ch;
    char buf[100];
    int i = 0, cmd_index = 0;
    // read file character by character
    while (read(fd, &ch, 1) > 0)
    {
        if (ch != '\n')
        {
            buf[i++] = ch; // store characters
        }
        else
        {
            buf[i] = '\0';                                          // end string                                      // terminate string
            external_commands[cmd_index] = malloc(strlen(buf) + 1); // allocate memory for command
            // copy command into array
            strcpy(external_commands[cmd_index], buf);
            cmd_index++;
            i = 0; // reset buffer index
        }
    }
    external_commands[cmd_index] = NULL; // mark end
    close(fd);
}