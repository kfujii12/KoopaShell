#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define MAX_SIZE 1024
#define MAX_COMMAND_ARGS 256
#define OVERWRITE 1
#define BACKGROUND_COMMAND 1
#define NO_BACKGROUND_COMMAND 0
#define FIRST_COMMAND_ARG 1
#define COMMAND 0 

int main(int argc, char* argv[])
{
    /* Variable declarations */
    char command[MAX_SIZE];
    char* commandArgs[MAX_COMMAND_ARGS]; /* Max number */
    int childPid = 0;
    int status;
    char currentDirectory[MAX_SIZE];
    char previousDirectory[MAX_SIZE];

    // Assume that there is no background command for now
    int backgroundCommand = NO_BACKGROUND_COMMAND;

    // Set the previous directory to the current one, since 
    // we just started the shell and have no 'previous directory'
    getcwd(previousDirectory, MAX_SIZE);

    while(1) /* Repeat forever */
    {
        // Get the current working directory
        getcwd(currentDirectory, MAX_SIZE);

        /* Display a prompt */
        printf("%s> %% ", currentDirectory);

        /* Read a command from the user */
        fgets(command, MAX_SIZE, stdin);

        /* Get rid of the new line character */
        command[strlen(command) - 1] = '\0';

        // Check to see if there is a & signifying a command 
        // running in the background
        if(!strcmp(&command[strlen(command) - 1], "&"))
        {
            // Set the background command flag
            backgroundCommand = BACKGROUND_COMMAND;

            // We know the command is meant to run in the background
            // Get rid of the & so that we can make a commandArgs 
            // without it 
            command[strlen(command) - 1] = '\0';
        }


        /* Parse the string into the command portion and
         * an array of pointers to argument strings using the blank
         * character as the separator. Note: the first argument
         * in the array of argument strings must be the same as
         * the command and the array must have a NULL pointer
         * after the last argument.
         */
        if (strlen(command))
        {
            // Start tokenizing the command string
            commandArgs[COMMAND] = strtok(command, " ");
     
            int i = 0;

            // Tokenize the command string until we get to a 
            // null terminator
            while (commandArgs[i] != NULL)
            {
                commandArgs[i + 1] = strtok(NULL, " ");
                i++;
            }

            /* If the user entered 'exit' then call the exit() system call
             * to terminate the process
             */
            if(!strcmp(commandArgs[COMMAND], "exit"))
            {
                exit(EXIT_SUCCESS);
            }

            /* If the command is a cd, then this is a built-in command.
             * The current functionality implements the 'cd', 'cd -', 
             * 'cd ~', and relative paths
             */
            else if (!strcmp(commandArgs[COMMAND], "cd"))
            {
                // Before we change directories, keep track of the current 
                // direcotry
                getcwd(currentDirectory, MAX_SIZE);

                /* If the command was 'cd' with no arguments, then toggle
                 * the current directory to be the previous directory
                 */ 
                if (!commandArgs[FIRST_COMMAND_ARG])
                {
                    // Get the previous directory
                    strcpy(previousDirectory, getenv("OLDPWD"));

                    // Set the new previous directory
                    setenv("OLDPWD", currentDirectory, OVERWRITE);

                    // Set the current directory to be the previous directory
                    chdir(previousDirectory);
                }

                // If the folder to change to is a ~, change to the home 
                // directory
                else if (!strcmp(commandArgs[FIRST_COMMAND_ARG], "~"))
                {
                    // Set the new previous directory
                    setenv("OLDPWD", currentDirectory, OVERWRITE);
                    
                    // Change value of current working directory to home
                    chdir(getenv("HOME"));
                }

                // If the folder to change to is -, then go back to the 
                // previous directory and print the new current directory
                else if (!strcmp(commandArgs[FIRST_COMMAND_ARG], "-"))
                {
                    // Set the previous directory
                    strcpy(previousDirectory, getenv("OLDPWD"));

                    // Set the new previous directory
                    setenv("OLDPWD", currentDirectory, OVERWRITE);

                    // Set the current directory to be the previous directory
                    chdir(previousDirectory);

                    // print the new directory 
                    printf("%s\n", previousDirectory);
                }
                else
                {
                    // Try to change value of current working directory
                    chdir(commandArgs[FIRST_COMMAND_ARG]);

                    // If it wasn't successful, then print an error message
                    if (errno)
                    {
                        fprintf(stderr, "No such file or directory\n");
                    }

                    // If it is successful, then update the oldpwd
                    setenv("OLDPWD", currentDirectory, OVERWRITE);

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

                if (!childPid) /* We forked no child, we ARE the child */
                {
                    /* We're now executing in the context of the child process.
                     * Use execvp() or execlp() to replace this program in 
                     * the process' memory with the executable command the 
                     * user has asked for.  
                     */

                    execvp(commandArgs[COMMAND], commandArgs); 

                    // Kill the child if it is unsuccessful in 
                    // executing its duties.
                    // Yeah, it's a bit harsh. 
                    if (errno) 
                    {
                        // Print error message
                        printf("%s\n", strerror(errno));

                        // Kill the child
                        exit(EXIT_SUCCESS);
                    }

                }
                else if (childPid == -1) 
                {

                    /* An error occured during the fork - print it */
                    printf("%s\n", strerror(errno));

                }
                else /* childPid is the PID of the child */
                {
                    /* We're still executing in the parent process.
                     * Wait for the child to finish before we prompt
                     * again.
                     */
                    
                    if (backgroundCommand)
                    {
                        printf("Job %d\n", childPid);
                        waitpid(childPid, &status, 0);

                        // After we are done waiting, then 
                        // reset the background command flag
                        backgroundCommand = NO_BACKGROUND_COMMAND;
                    }
                    else
                    {
                        // Wait for the child to finish before we prompt again
                        wait(&status);
                    }
                }
            }

        }

    } /* while */

} /* my shell */ 
