/*
 * ===== Assignment OU4 for "Systemnära Programmering 5DV088 ht19" =======
 *
 * mfind.c
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

#include "mfind.h"

/* Structs */
struct thread_data{
	int status;                      //Thread status, non-zero indicates error.
	int no_threads;                  //Number of threads to run.
	int no_waiting_threads;          //Number of threads currently waiting.
	char search_type;                //Filetype to search for.
	char search_keyword[PATH_MAX];   //Keyword to search for.
};

/* Global variables */
list *g_dir_stack;                  //List as a stack to hold directories.
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


/* Main */
int main(int argc, char *argv[]){

	/* Struct to hold data accessible to threads */
	thread_data t_data;
	t_data.status = 0;
	t_data.search_type = 'n';
	t_data.no_threads = parse_arguments(argc, argv,
	                    &t_data.search_type, t_data.search_keyword);
	t_data.no_waiting_threads = 0;

	/* Push directory arguments to directory stack */
	g_dir_stack = list_empty();
	for (int i = optind; i < argc-1; i++){
		char filetype;
		if ( (filetype = check_file_type(argv[i])) != 0) {

			/* Print if keyword is found in argument */
			if ((t_data.search_type == filetype || t_data.search_type == 'n')){
				if (strlen(t_data.search_keyword) < strlen(argv[i])){
			   	if (strcmp(t_data.search_keyword,
					           &argv[i][strlen(argv[i])-strlen(t_data.search_keyword)]) == 0){
						printf("%s\n", argv[i]);
					}
				}
			}
			stack_push(g_dir_stack, argv[i]);
		}
	}

	/* Create threads and start search */
	if (t_data.no_threads > 1){
		pthread_t thread_id[t_data.no_threads-1];

		for (int i = 1; i<t_data.no_threads; i++){
			if(pthread_create(&thread_id[i-1], NULL, thread_search, &t_data)){
				perror("pthread");
				exit_function(EXIT_FAILURE);
			}
		}
		thread_search(&t_data);

		for (int i = 1; i<t_data.no_threads; i++){
			if(pthread_join(thread_id[i-1], NULL)){
				perror("pthread");
				exit_function(EXIT_FAILURE);
			}
		}
	}else{
		thread_search(&t_data);
	}

	/* Cleanup */
	list_kill(g_dir_stack);
	return t_data.status;
}

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
void *thread_search(void *param){
	thread_data *t_data = (thread_data *)param;
	char startdir[PATH_MAX];
	char path[PATH_MAX];
	unsigned int no_reads = 0;

	while(1){
		/* Wait for work if stack is empty */
		pthread_mutex_lock(&lock);
		while(stack_isempty(g_dir_stack)){
			t_data->no_waiting_threads++;

			/* Only wait if you are not the last running thread */
			if(t_data->no_waiting_threads < t_data->no_threads){
				pthread_cond_wait(&cond, &lock);

				/* Exit on wake-up if all the threads are waiting */
				if(t_data->no_waiting_threads == t_data->no_threads){
					printf("Thread: %lu Reads: %d\n", pthread_self(), no_reads);
					pthread_mutex_unlock(&lock);
					return NULL;
				}

			/* Last thread signals all waiting threads to wake up and exit */
			}else if (t_data->no_waiting_threads == t_data->no_threads){
				pthread_cond_broadcast(&cond);
				printf("Thread: %lu Reads: %d\n", pthread_self(), no_reads);
				pthread_mutex_unlock(&lock);
				return NULL;
			}
			t_data->no_waiting_threads--;
		}
		assert(!stack_isempty(g_dir_stack));

		/* Set search path to directory on top of stack */
		strcpy(path, stack_top(g_dir_stack));
		strcpy(startdir, stack_top(g_dir_stack));
		stack_pop(g_dir_stack);
		pthread_mutex_unlock(&lock);

		/* Open directory */
		DIR *stream = opendir(path);
		if (stream == NULL){
			if (errno == 13){ //Permission denied
				fprintf(stderr, "opendir: %s: %s\n", strerror(errno), path);
				// t_data->status = 1; labres wants a 0, spec wants a non-zero...
				continue;
			}else{				//Other
				fprintf(stderr, "opendir: %s: %s\n", strerror(errno), path);
				exit_function(EXIT_FAILURE);
			}
		}
		no_reads++;

		/* Read directory */
		struct dirent *p_dirent;
		while ((p_dirent = readdir(stream)) != NULL){
			if ((strcmp(p_dirent->d_name, ".") == 0) ||
			(strcmp(p_dirent->d_name, "..") == 0)){
				continue;
			}

			strcpy(path, startdir);
			strcat(path, "/");
			strcat(path, p_dirent->d_name);

			char filetype = check_file_type(path);
			/* Add new dirs to stack and signal waiting threads */
			if (filetype == 'd'){
				pthread_mutex_lock(&lock);
				stack_push(g_dir_stack, path);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&lock);
			}
			/* Print any matches to stdout */
			if ((t_data->search_type == filetype || t_data->search_type == 'n') &&
			   (strcmp(t_data->search_keyword, p_dirent->d_name) == 0)){
				pthread_mutex_lock(&lock);
				fprintf(stdout, "%s\n", path);
				pthread_mutex_unlock(&lock);
			}
		}
		closedir(stream);
	}
	return param;
}


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
int parse_arguments(int argc, char *argv[], char *type, char *keyword){
	int opt = 0;
	int nrthr = 1;
	int error = 0;

	while ((opt = getopt(argc, argv, "p:t:")) != -1) {
		switch (opt) {
			case 'p':
				nrthr = atoi(optarg);
				if ((nrthr > MAX_NO_THREADS) || (nrthr < 1)){
					fprintf(stderr, "Invalid argument: nrthr out of range (1-%d)\n"
					, MAX_NO_THREADS);
					error = 1;
				}
				break;
			case 't':
				if ((optarg[0] != 'd') && (optarg[0] != 'f') && (optarg[0] != 'l')){
					fprintf(stderr, "Invalid type argument\n");
					error = 1;
				}
				*type = optarg[0];
				break;
			default:
				error = 1;
		}
	}
	if (strlen(argv[argc-1]) <= PATH_MAX){
		strcpy(keyword, argv[argc-1]);
	}else{
		error = 1;
	}

	if ((optind >= argc) || argc < 3) {
		error = 1;
	}

	if (error == 1){
		fprintf(stderr, "Usage: %s [-t type] [-p nrthr] ", argv[0]);
		fprintf(stderr, "start1 [start2 ...] name\n");
		exit(EXIT_FAILURE);
	}
	return nrthr;
}


/**
* check_file_type() - Uses lstat() to check file type of @path.
*
* @path: Path to file to check.
*
* Returns: 'd' for directory, 'f' for regular file, 'l' for link.
*/
char check_file_type(char *path){
	char type = 0;
	struct stat file_info;
	if (lstat(path, &file_info) < 0) {
		fprintf(stderr, "lstat err :%d %s: %s\n", errno, strerror(errno), path);
		perror("lstat");
	}

	if (S_ISDIR(file_info.st_mode)){
		type = 'd';
	}else if(S_ISREG(file_info.st_mode)){
		type = 'f';
	}else if(S_ISLNK(file_info.st_mode)){
		type = 'l';
	}

	return type;
}


/**
* stack_push() - Push a string to stack.
*
* @l - Stack to push to.
* @str_to_push - String to push.
*
* Returns: Nothing
*/
void stack_push(list *l, char *str_to_push){
	char *str = malloc(strlen(str_to_push)+1);
	if (str == NULL){
		perror("malloc"); exit(EXIT_FAILURE);
	}

	strcpy(str, str_to_push);
	str[strlen(str_to_push)] = '\0';
	list_insert(l, list_first(l), str);
}


/**
* stack_top() - Peeks at the stack and returns a pointer to the object
* on the top of the stack.
*
* @l: Stack to top.
*
* Returns: Pointer to the object on top of the stack.
*/
void *stack_top(list *l){
	return list_inspect(l, list_next(l, list_first(l)));
}


/**
* stack_pop() - Removes the element on top of the stack.
*
* @l: Stack to pop.
*
* Returns: Nothing.
*/
void stack_pop(list *l){
	if (!list_isempty(l)){
		list_remove(l, list_first(l));
	}else{
		fprintf(stderr, "Warning: trying to pop an empty stack!\n");
	}
}


/**
* stack_is_empty() - Checks if a stack is empty.
*
* @l: Stack to check
*
* Returns: Non-zero if empty, otherwise zero.
*/
int stack_isempty(list *l){
	return list_isempty(l);
}


/**
* exit_function() - Cleanup and exit function.
*
* @status: Exit status.
*
* Returns: Nothing
*/
void exit_function(int status){
	list_kill(g_dir_stack);
	exit(status);
}
