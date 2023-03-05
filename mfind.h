#ifndef _MFIND_
#define _MFIND_
/*
 * ===== Assignment OU4 for "Systemnära Programmering 5DV088 ht19" =======
 *
 * mfind.h
 *
 * This program is an implementation of the UNIX-command find with limited
 * functionality. It searches for a specified file, link or directory in the
 * path supplied by the user by looking in all subdirectories to the path.
 * If a match is found to the specified file the full path to that file is
 * printed to stdout. The program can use multiple threads to increase the
 * performance when searching through big directories.
 *
 * Author: Mattias Lindström
 * Mail: hed17mlm@cs.umu.se
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "list.h"

/* Constans */
#define MAX_NO_THREADS 500

/* Typedef */
typedef struct thread_data thread_data;

/* Global variables */
extern list *g_dir_stack;
extern pthread_mutex_t lock;
extern pthread_cond_t cond;

/* Function Prototypes */
/**
* thread_search() - Entry function for threads.
* The function takes the directory on top of the directory-stack and searches
* for files matching the user specified keyword. If subdirectories are found
* they are added to the directory-stack and will be searched through until
* the stack is empty and all running threads are in the waiting state.
*
* @param: Argument passed to thread function. Used for sending a thread_data
* struct to the thread function.
*
* Returns: NULL.
*/
void *thread_search(void *param);


/**
* parse_arguments() - Parses program arguments by using the getopt() function.
* Options include: -p for setting the number of threads to run.
*                  -t for specifying file type to search for.
*
* @argc: Argument count.
* @argv: Argument vector.
* @type: String to hold search type.
* @keyword: String to hold search keyword.
*
* Returns: Number of threads to run.
*/
int parse_arguments(int argc, char *argv[], char *type, char *keyword);


/**
* check_file_type() - Uses lstat() to check file type of @path.
*
* @path: Path to file to check.
*
* Returns: 'd' for directory, 'f' for regular file, 'l' for link.
*/
char check_file_type(char *path);


/**
* stack_push() - Push a string to stack.
*
* @l - Stack to push to.
* @str_to_push - String to push.
*
* Returns: Nothing
*/
void stack_push(list *l, char *str_to_push);


/**
* stack_top() - Peeks at the stack and returns a pointer to the object
* on the top of the stack.
*
* @l: Stack to top.
*
* Returns: Pointer to the object on top of the stack.
*/
void *stack_top(list *l);


/**
* stack_pop() - Removes the element on top of the stack.
*
* @l: Stack to pop.
*
* Returns: Nothing.
*/
void stack_pop(list *l);


/**
* stack_is_empty() - Checks if a stack is empty.
*
* @l: Stack to check
*
* Returns: Non-zero if empty, otherwise zero.
*/
int stack_isempty(list *l);


/**
* exit_function() - Cleanup and exit function.
*
* @status: Exit status.
*
* Returns: Nothing
*/
void exit_function(int status);

#endif
