#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SIZE 1024

int main(int argc, char* argv[])
{
    /* Variable declarations */
    char command[MAX_SIZE];
    char* commandArgs[256]; /* Max number */
    int childPid = 0;
    int status;

    while(1) /* Repeat forever */
    {
        /* Display a prompt */
        printf("KoopaShell> %% ");

        /* Read a command from the user */
        fgets(command, MAX_SIZE, stdin);

        /* Get rid of the new line character */
        command[strlen(command) - 1] = '\0';

        /* Parse the string into the command portion and
         * an array of pointers to argument strings using the blank
         * character as the separator. Note: the first argument
         * in the array of argument strings must be the same as
         * the command and the array must have a NULL pointer
         * after the last argument.
         */
        commandArgs[0] = strtok(command, " ");
 
        int i = 0;

        while (commandArgs[i] != NULL)
        {
            commandArgs[i + 1] = strtok(NULL, " ");
            i++;
        }



        /* If the user entered 'exit' then call the exit() system call
         * to terminate the process
         */
        if(!strcmp(commandArgs[0], "exit"))
        {
            exit(EXIT_SUCCESS);
        }
        
        /* Fork a child process to execute the command and return 
         * the result of the fork() in the childPid variable so 
         * we know whether we're now executing as the parent 
         * or child and whether or not the fork() succeeded
         */
        childPid = fork();
       
        /* TODO: Implement the cd command, allow for & */

        if (!childPid) /* We forked no child, we ARE the child */
        {
            /* We're now executing in the context of the child process.
             * Use execvp() or execlp() to replace this program in 
             * the process' memory with the executable command the 
             * user has asked for.  
             */
            // Check to see if the command exists as a shell command already
            execvp(commandArgs[0], commandArgs); 

            // Check to see if there is an ampersand appended to the command
            
        }
        else if (childPid == -1) 
        {
            /* An error occured during the fork - print it */
            printf("An error occured during the process: %d", childPid);

        }
        else /* childPid is the PID of the child */
        {
            /* We're still executing in the parent process.
             * Wait for the child to finish before we prompt
             * again.
             */
            wait(&status);
        }

        

    } /* while */

} /* my shell */ 
