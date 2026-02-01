#include "systemcalls.h"
#include "fcntl.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    /*
    printf("  Executing: %s\n", cmd);
    int return_num = system(cmd);

    if (cmd == NULL)
    {
        if (return_num == 0)
        {
            // No shell is available
            return false; 
        }
    }
    else if (return_num == -1)
    {
        // child process could not be created or its status could not be retrieved
        return false; 
    }
    else if (return_num == 127)
    {
        // shell could not be executed in the child process
        return false;
    }
    else if (return_num != 0)
    {
        // cmd command returned non-zero
        return false;
    }

    */

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{

    /*
    if (count < 1)
    {
        printf("  Error: do_exec() requires at least one argument besides count to work!");
        return false;
    }

    va_list args;
    va_start(args, count);
    
    const char * path = va_arg(args, char *);
    if(path[0] != '/')
    {
        printf("  Error: '%s' is not a absolute path!\n", path);
        return false;
    }

    char * argv[count];
    char * argument = NULL;

    for(int i = 0; i < count - 1; i++)
    {
        argument = va_arg(args, char *);
        if (argument[0] != '-' && argument[0] != '/')
        {
            printf("  Error: Argument '%s' is not expanded!\n", argument);
            return false;
        }
        argv[i] = argument;
    }
    argv[count - 1] = NULL;


    

    struct stat file_stat;
    int stat_returned = stat(path, &file_stat);
    if (stat_returned == -1)
    {
        printf("  Error: '%s' is not a valid path to a file to execute!\n", path);
        return false;
    }

    pid_t fork_pid = fork();
    if (fork_pid == -1)
    {
        printf("  Error: Failed to create child proccess!\n");
        return false;
    }
    else if (fork_pid == 0)
    {
        // Child logic
        printf("  Child executing: %s", path);
        for (int i = 0; argv[i] != NULL; i++) 
        {
            printf("   %s", argv[i]);
        }
        printf("  \n");

        int execv_return = execv(path, (char * const*) argv);
        if (execv_return == -1)
        {
            printf("  Error child failed to run command\n");
            return false;
        }
    }
    else
    {
        // Parent logic
        int wstatus;
        pid_t waitpid_return = waitpid(fork_pid, &wstatus, WUNTRACED);
        if (waitpid_return == -1)
        {
            printf("  Error: Failed to wait for child %d!", fork_pid);
            return false;
        }
        else if (!WIFEXITED(wstatus))
        {
            printf("  Error: Child process did not exit normally\n");
            return false;
        }
        else if (WEXITSTATUS(wstatus) != 0)
        {
            printf("  Error: Child process did not return 0\n");
            return false;
        }
    }

    va_end(args);
    */

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    /*
    va_list args;
    va_start(args, count);

    int fd = open(outputfile, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (fd == -1)
    {
        printf("  Error: Failed to open file descriptor for %s\n", outputfile);
        return false;
    }

    int dup_return = dup2(fd, 1);
    if (dup_return == -1)
    {
        printf("  Error: Failed to duplicated standard output to %s\n", outputfile);
        return false;
    }

    int close_return = close(fd);
    if (close_return == -1)
    {
        printf("  Error: Failed to close %s file\n", outputfile);
        return false;
    }

    bool do_exec_return = do_exec(count, args);

    va_end(args);

    return do_exec_return;
    */

    return true;
}
