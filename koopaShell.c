#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SIZE 1024
#define MAX_COMMAND_ARGS 256
#define OVERWRITE 1
#define BACKGROUND_COMMAND 1
#define NO_BACKGROUND_COMMAND 0

int main(int argc, char* argv[])
{
    //TODO: Errors
    /* Variable declarations */
    char command[MAX_SIZE];
    char* commandArgs[MAX_COMMAND_ARGS]; /* Max number */
    int childPid = 0;
    int status;
    char currentDirectory[MAX_SIZE];
    char previousDirectory[MAX_SIZE];

    // Assume that there is no background command for now
    int backgroundCommand = NO_BACKGROUND_COMMAND;

    getcwd(previousDirectory, MAX_SIZE);

    while(1) /* Repeat forever */
    {
        getcwd(currentDirectory, MAX_SIZE);

        /* Display a prompt */
        printf("%s> %% ", currentDirectory);

        /* Read a command from the user */
        fgets(command, MAX_SIZE, stdin);

        /* Get rid of the new line character */
        command[strlen(command) - 1] = '\0';

        // Check to see if there is a & signifying a command 
        // running in the background
        // if(!strcmp(&command[strlen(command) - 1], "&"))
        // {
        //     backgroundCommand = BACKGROUND_COMMAND;

        //     // We know the command is meant to run in the background
        //     // Get rid of the & so that we can make a commandArgs 
        //     // without it 
        //     command[strlen(command) - 1] = '\0';
        // }


        /* Parse the string into the command portion and
         * an array of pointers to argument strings using the blank
         * character as the separator. Note: the first argument
         * in the array of argument strings must be the same as
         * the command and the array must have a NULL pointer
         * after the last argument.
         */
        if (strlen(command))
        {

            commandArgs[0] = strtok(command, " ");
     
            int i = 0;

            while (commandArgs[i] != NULL)
            {
                commandArgs[i + 1] = strtok(NULL, " ");
                i++;
            }

            // See if the command is a built-in function

            /* If the user entered 'exit' then call the exit() system call
             * to terminate the process
             */


            if(!strcmp(commandArgs[0], "exit"))
            {
                exit(EXIT_SUCCESS);
            }

            // If the command is a cd, then this is a built-in command
            else if (!strcmp(commandArgs[0], "cd"))
            {
                // Syntax is:
                // cd [option] [directory]
                // Needs to support absolute and relative directory
                // PWD environement variable is present working directory

                // Before we change directories, keep track of the current direcotry
                getcwd(currentDirectory, MAX_SIZE);

                // If the folder to change to is a ~, change to the home directory
                if (!strcmp(commandArgs[1], "~"))
                {
                    setenv("OLDPWD", currentDirectory, OVERWRITE);
                    
                    // Change value of current working directory
                    chdir(getenv("HOME"));
                }


                // If the folder to change to is -, then go back to the previous directory
                if (!strcmp(commandArgs[1], "-"))
                {

                    strcpy(previousDirectory, getenv("OLDPWD"));

                    setenv("OLDPWD", currentDirectory, OVERWRITE);

                    chdir(previousDirectory);
                }
                else
                {
                    setenv("OLDPWD", currentDirectory, OVERWRITE);

                    // Change value of current working directory
                    chdir(commandArgs[1]);
                }

            
            }

            else
            {

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

                    execvp(commandArgs[0], commandArgs); 

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
                    
                    // if (backgroundCommand)
                    // {
                    //     wait(&status);
                    // }
                    wait(&status);
                }
            }

            

        }

        

    } /* while */

} /* my shell */ 
