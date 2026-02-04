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
    else if (!WIFEXITED(return_num))
    {
        // Shell should have exited
        return false; 
    }
    else if (WEXITSTATUS(return_num) == 127)
    {
        // shell could not be executed in the child process
        return false;
    }
    else if (WEXITSTATUS(return_num) != 0)
    {
        // cmd command returned non-zero
        return false;
    }

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
    if (count < 1)
    {
        printf("  Error: do_exec() requires at least one argument besides count to work!");
        return false;
    }

    va_list args;
    va_start(args, count);
    char * command[count+1];
    char * argument = NULL;
    char * const* argv = NULL;
    for(int i = 0; i < count; i++)
    {
        argument = va_arg(args, char *);
        if (i == 0 && argument[0] != '/')
        {
            printf("  Error: File '%s' is not expanded!\n", argument);
            va_end(args);
            return false;
        }
        command[i] = argument;
    }
    command[count] = NULL;
    argv = (char * const*) command;

    pid_t fork_pid = fork();
    if (fork_pid == -1)
    {
        printf("  Error: Failed to create child proccess!\n");
        va_end(args);
        return false;
    }
    else if (fork_pid == 0)
    {
        // Child logic
        int execv_return = execv(argv[0], argv);
        if (execv_return == -1)
        {
            perror("execv");
            _exit(127);
        }
    }
    else
    {
        // Parent logic
        int wstatus;
        pid_t waitpid_return = waitpid(fork_pid, &wstatus, 0);
        if (waitpid_return == -1)
        {
            printf("  Error: Failed to wait for child %d!", fork_pid);
            va_end(args);
            return false;
        }
        else if (!WIFEXITED(wstatus))
        {
            printf("  Error: Child process did not exit normally\n");
            va_end(args);
            return false;
        }
        else if (WEXITSTATUS(wstatus) != 0)
        {
            printf("  Error: Child process did not return 0\n");
            va_end(args);
            return false;
        }
    }

    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    if (count < 1)
    {
        printf("  Error: do_exec() requires at least one argument besides count to work!");
        return false;
    }

    va_list args;
    va_start(args, count);
    char * command[count+1];
    char * argument = NULL;
    char * const* argv = NULL;
    for(int i = 0; i < count; i++)
    {
        argument = va_arg(args, char *);

        if (argument == NULL)
        {
            printf("  Error: count '%d' was not populated correctly!\n", count);
            va_end(args);
            return false;
        }
        else if (i == 0 && argument[0] != '/')
        {
            printf("  Error: File '%s' is not expanded!\n", argument);
            va_end(args);
            return false;
        }
        command[i] = argument;
    }
    command[count] = NULL;
    argv = (char * const*) command;

    pid_t fork_pid = fork();
    if (fork_pid == -1)
    {
        printf("  Error: Failed to create child proccess!\n");
        va_end(args);
        return false;
    }
    else if (fork_pid == 0)
    {
        // Child logic
        int fd = open(outputfile, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1)
        {
            _exit(127);
        }

        int dup_return = dup2(fd, STDOUT_FILENO);
        if (dup_return == -1)
        {
            close(fd);
            _exit(127);
        }

        int close_return = close(fd);
        if (close_return == -1)
        {
            _exit(127);
        }

        int execv_return = execv(argv[0], argv);
        if (execv_return == -1)
        {
            perror("execv");
            _exit(127);
        }
    }
    else
    {
        // Parent logic
        int wstatus;
        pid_t waitpid_return = waitpid(fork_pid, &wstatus, 0);
        if (waitpid_return == -1)
        {
            printf("  Error: Failed to wait for child %d!", fork_pid);
            va_end(args);
            return false;
        }
        else if (!WIFEXITED(wstatus))
        {
            printf("  Error: Child process did not exit normally\n");
            va_end(args);
            return false;
        }
        else if (WEXITSTATUS(wstatus) != 0)
        {
            printf("  Error: Child process did not return 0\n");
            va_end(args);
            return false;
        }
    }
    
    va_end(args);

    return true;
}
