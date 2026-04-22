/*
NAME        : V.V.DURGADEVI
DATE        : 6 APRIL 2026
DESCRIPTION : This project implements a simple Linux command-line shell called MINI SHELL,
              which allows users to enter and execute commands just like Bash. 
              It supports features such as command execution, built-in commands, signal handling, background processes, and piping
*/
#include "header.h"
extern pid_t c1;    // c1 stores current foreground process pid
int job_id = 0;     // job counter
extern Slist *head; // head of linked list

/*void error_handler(int sig_num){
    printf("Error handler -> %d\n", sig_num);
    perror("malloc");
    exit(1);
}*/

void signal_handler(int sig_num)
{
    // ===== BACKGROUND PROCESS FINISHED =====
        if (sig_num == SIGCHLD)
        {
            // used for background process completion
            pid_t pid;
            // loop to collect all finished child processes
            while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
            {
                // remove last job from list
                sl_delete_last(&head);
            }
        }
    if (sig_num == SIGINT) // Ctrl + C Handling
    {
        if (c1 > 0) // if process is going on and ctrl+c occurs it terminate the child process
        {
            // kill(c1, SIGINT);
            printf("\n");
        }
        else
        { // if no process is running just print prompt
            printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET, prompt);
            fflush(stdout);
        }
    }
    else if (sig_num == SIGTSTP) // Ctrl + Z handling
    {
        if (c1 > 0) // if the foreground process is running
        {
            // stop the running process
            // kill(c1, SIGTSTP);

            Slist *temp = head; // pointer to traverse list
            int found = 0;     
            // Traverse job list to check if this PID already exists
            while (temp)
            {
                if (temp->pid == c1)
                {
                    // If already present
                    temp->status = STOPPED; // update status
                    found = 1;              // mark as found
                    break;
                }
                temp = temp->link; // move to next node
            }

            // If job is not already in list insert it into the linkedin list
            if (!found)
            {
                insert_at_last(&head, c1);
            }
            // print job details
            if (input_string[0] != '\0')
                printf(ANSI_COLOR_CYAN"\n[%d]+ Stopped\t %s\n"ANSI_COLOR_RESET, job_id, input_string);
            else
                printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET, prompt);
            c1 = 0; // Reset since no foreground process now
        }
        else
        {
            printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET, prompt); // no process happens just print the prompt
            fflush(stdout);
        }
    }
}
void fg(Slist **head)
{
    // if no jobs available
    if (*head == NULL)
    {
        printf(ANSI_COLOR_MAGENTA"-bash: fg: current: no such job\n"ANSI_COLOR_RESET);
        return;
    }

    Slist *temp = *head;
    // traverse to last node (we have to resume the last process)
    while (temp->link != NULL)
    {
        temp = temp->link;
    }
    strcpy(input_string, temp->input_str); // restore command
    printf(ANSI_COLOR_YELLOW"%s\n"ANSI_COLOR_RESET, temp->input_str);       // print command

    c1 = temp->pid;           // set as foreground process
    kill(temp->pid, SIGCONT); // resume process

    temp->status = RUNNING; // update status

    waitpid(temp->pid, &status, WUNTRACED); // wait for completion/stop

    c1 = 0; // reset after execution
    // check if process stopped again
    if (WIFSTOPPED(status))
    {
        // process stopped again keep it in list
        temp->status = STOPPED;
    }
    else
    {
        // process finished  remove from the list
        sl_delete_last(head);
    }
}
void bg(Slist **head)
{
    // if no jobs
    if (*head == NULL)
    {
        printf(ANSI_COLOR_MAGENTA"-bash: bg: current: no such job\n"ANSI_COLOR_RESET);
        return;
    }

    Slist *temp = *head;

    // go to last job  to run the job in background
    while (temp->link != NULL)
    {
        temp = temp->link;
    }

    kill(temp->pid, SIGCONT); // resume process in background

    temp->status = RUNNING; // update status

    printf(ANSI_COLOR_YELLOW"[%d]+ %s\n"ANSI_COLOR_RESET, temp->job_num, temp->input_str);
}
int insert_at_last(Slist **head, int pid)
{
    // create new node
    //printf("before malloc\n");
    Slist *new_node = malloc(sizeof(Slist) + 5);
    //printf("after malloc\n");
    if (new_node == NULL)
        return -1;
    new_node->pid = pid; // store the pid
    // store command string
    //printf("Input string -> %s\n", input_string);
    new_node->input_str = malloc(strlen(input_string) + 1);
    strcpy(new_node->input_str, input_string);
    new_node->job_num = ++job_id; // assign job number
    new_node->status = STOPPED;   // initial status
    new_node->link = NULL;
    // insert at end
    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        Slist *temp = *head;
        while (temp->link != NULL)
        {
            temp = temp->link;
        }
        temp->link = new_node;
    }
    return 0;
}
int sl_delete_last(Slist **head)
{
    if (*head == NULL)
        return -1;
    // if only one node
    if ((*head)->link == NULL)
    {
        free((*head)->input_str);
        free(*head);
        *head = NULL;
        job_id = 0; // reset job counter(job_id)
    }
    else
    {
        Slist *temp = *head;
        // go to second last node
        while (temp->link->link != NULL)
        {
            temp = temp->link;
        }
        free(temp->link->input_str);
        free(temp->link);
        temp->link = NULL;
        job_id--;
    }
    return 0;
}
void print_list(Slist *head)
{
    if(head == NULL)
    {
        printf(ANSI_COLOR_CYAN"No jobs\n"ANSI_COLOR_RESET);
        return;
    }
    // print based on status
    while (head)
    {
        if (head->status == STOPPED)
            printf(ANSI_COLOR_CYAN"[%d]+ Stopped\t %s\n"ANSI_COLOR_RESET, head->job_num, head->input_str);
        else
            printf(ANSI_COLOR_CYAN"[%d]+ Running\t %s\n"ANSI_COLOR_RESET, head->job_num, head->input_str);
        head = head->link;
    }
}