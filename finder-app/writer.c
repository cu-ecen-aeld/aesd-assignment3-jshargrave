/*
    Author: Joseph Hargrave
    Date: 1/17/2026
    Requirements:
        Write a C application “writer” (finder-app/writer.c)  which can be used as an alternative to the “writer.sh” test script created in assignment1 and using File IO as described in LSP chapter 2.  See the Assignment 1 requirements for the writer.sh test script and these additional instructions:
    
        Accepts the following arguments: the first argument is a full path to a file (including filename) on the filesystem, referred to below as writefile; the second argument is a text string which will be written within this file, referred to below as writestr

        Exits with value 1 error and print statements if any of the arguments above were not specified

        Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating the path if it doesn’t exist. Exits with value 1 and error print statement if the file could not be created.

        One difference from the write.sh instructions in Assignment 1:  You do not need to make your "writer" utility create directories which do not exist.  You can assume the directory is created by the caller.

        Setup syslog logging for your utility using the LOG_USER facility.

        Use the syslog capability to write a message “Writing <string> to <file>” where <string> is the text string written to file (second argument) and <file> is the file created by the script.  This should be written with LOG_DEBUG level.

        Use the syslog capability to log any unexpected errors with LOG_ERR level.
*/

#include "stdio.h"
#include <syslog.h>
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// File flags: write only and create file if it does not exist
#define FILE_FLAGS O_WRONLY | O_CREAT
// File mode: User has read/write, groupd has read/write, everyone has read permissions
#define FILE_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

void print_and_log(int log_level, char* format, char* content)
{
    printf(format, content);
    syslog(log_level, format, content);
}

void print_and_log2(int log_level, char* format, char* content1, char* content2)
{
    printf(format, content1, content2);
    syslog(log_level, format, content1, content2);
}

int main(int argc, char *argv[])
{
    // Setup logger
    openlog(NULL, LOG_ODELAY, LOG_USER);

    // If missing writefile
    if (argc < 2)
    {
        print_and_log(LOG_ERR, "%s\n", "Error: Missing writefile argument!");
        return 1;
    }

    // If missing writestr
    if (argc < 3)
    {
        print_and_log(LOG_ERR, "%s\n", "Error: Missing writestr argument!");
        return 1;
    }

    // Create and open file

    char* writefile = argv[1];
    char* writestr = argv[2];
    int file_descriptor = open(writefile, FILE_FLAGS, FILE_MODE);
    
    // Check that file opened correctly
    if (file_descriptor == -1)
    {
        print_and_log(LOG_ERR, "Error: Failed to open file '%s'!\n", writefile);
        return 1;
    }

    // Log write
    print_and_log2(LOG_DEBUG, "Writing '%s' to '%s'\n", writestr, writefile);

    // Write contents to file
    size_t write_length = strlen(writestr);
    ssize_t bytes_written = write(file_descriptor, writestr, write_length);
    if (bytes_written == -1)
    {
        print_and_log(LOG_ERR, "Error: Failed to write to file '%s'!\n", writefile);
        return 1;
    }
    else if (bytes_written != write_length)
    {
        print_and_log(LOG_ERR, "Error: Failed to write all content to file '%s'!\n", writefile);
        return 1;
    }

    // Close file
    int close_return = close(file_descriptor);
    if (close_return == -1)
    {
        print_and_log(LOG_ERR, "Error: Failed to close file '%s'!\n", writefile);
        return 1;
    }
    
    // Close log
    closelog();
    
    return 0;
}

