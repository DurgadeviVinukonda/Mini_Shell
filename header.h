#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
// LINKED LIST NODE STRUCTURE
typedef struct node
{
	int job_num;	   // job number (like [1], [2])
	int status;		   // RUNNING or STOPPED
	char *input_str;   // RUNNING or STOPPED
	pid_t pid;		   // process id
	struct node *link; // pointer to next node
} Slist;
#define RUNNING 1
#define STOPPED 0
#define BUILTIN 1
#define EXTERNAL 2
#define NO_COMMAND 3

// COLOR MACROS (for output formatting)
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

// extern variables
extern char *external_commands[200];
extern char *builtins[];
extern int status;
extern char prompt[20];
extern char input_string[100];

void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
int npipe(int argc, char *argv[]);
int check_command_type(char *command);
void execute_internal_commands(char *input_string);
void execute_external_commands(char *input_string);
void signal_handler(int sig_num);
void error_handler(int sig_num);
void extract_external_commands(char **external_commands);
void fg(Slist **head);
void bg(Slist **head);
int insert_at_last(Slist **head, int data);
int sl_delete_last(Slist **head);
void print_list(Slist *head);
#endif