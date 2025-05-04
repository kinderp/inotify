#include <stdio.h>	 	/* input-output 	*/
#include <stdlib.h>	 	/* exit() 		*/
#include <string.h>	 	/* strcmp() strcpy() 	*/
#include <unistd.h>	 	/* read() 		*/
#include <sys/inotify.h> 	/* all inotify calls 	*/
#include <sys/stat.h>    	/* stat() 		*/
#include <limits.h>     	/* NAME_MAX, PATH_MAX	*/
#include <dirent.h>     	/* scandir() 		*/
#include <time.h>		/* time()		*/
#include <errno.h>		/* strerror(errno)	*/
#include <signal.h>
#include "monitor.h"		/* display_inotify_event() */
#include "print.h"		/* print_progress()	*/
				/* print_logo()		*/
#include "index.h"		/* index_process()	*/
#include "util.h"		/* get_timestamp()	*/

#define IS_DIR(x) x == 4 ? 1 : 0

#define EREPORT_DIM  3
#define EREPORT_INC_DIM  2

int EREPORT_DIM_REAL = EREPORT_DIM;

char *LOGO_FILENAME = "logo.txt";
char *LOGS_FILENAME = "logs.txt";

FILE *LOGS = NULL;

struct queue_event **EREPORT = NULL;
char **TRIGGERED_NAMES = NULL;

/*
 * 1. I need to define a right dim for inotify input buffer
 *    In this buffer, we're gonna  read triggered events on
 *    watched files/direcotories  
 */

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))


volatile sig_atomic_t stop;

void inthand(int signum) {
    printf("CTRL-C");
    stop = 1;
}

int main(int argc, char *argv[]){

	init_queues_system(EREPORT_DIM, 0);

	signal(SIGINT, inthand);

	if (argc < 2 || strcmp(argv[1], "--help") == 0){
        	printf("%s pathname...\n", argv[0]);
		exit(1);
    	}	

	print_logo();
	 
	LOGS = fopen(LOGS_FILENAME, "w");
	if(LOGS == NULL){
		printf("Can't open %s, quitting!", LOGS_FILENAME);
		exit(1);
	}
	/*
	 * inotify_fd: it's a file descriptor and is an handle that is 
	 *	       used to refer to the inotify instance in subseq
	 *	       uent operations (e.g. inotify_add_watch() ). It
	 *             will store fd returned back from inotify_init(). 
	 * 	       We will read events with  read() function using 
	 *	       this file descriptor.
	 */
	int inotify_fd;

	/*
         * input_buffer: it's a buffer that we will pass to read() as
	 *		 input argument. Each read operations will st
	 *		 ore triggered inotify events on this buffer. 
	 * 
         */
	char input_buffer[BUF_LEN];
	
	struct inotify_event *event;

	inotify_fd = inotify_init();
	if (inotify_fd == -1){
		printf("Error on inotify_init()");
        	exit(1);
    	}

	struct dirent **namelist;
	struct watched_dir *head = (struct watched_dir *)malloc(sizeof(struct watched_dir));
	head->name = argv[1];
	head->next = NULL;

	int num_files = 0;
	int num_dirs = 0; 
       	num_files = scandir(argv[1], &namelist, NULL, alphasort);

	char *root_path = (char *)malloc(PATH_MAX*sizeof(char));
	realpath(argv[1], root_path);
	strcat(root_path, "/");
	struct stat sb;
	time_t start = get_timestamp();
	indexing_process(root_path, &head, &num_dirs);
	time_t stop = get_timestamp(); 
	printf("\n ** Indexing process took %ld sec(s)\n", stop - start);

       	/*
         * TODO: We need to manage scandir error: n < 0
         */

	/*
	 * IN_CREATE:
	 *	Description: File/directory created inside watched directory
	 *	Action	   : Add a new watch only if it's this new dir
	 *
	 * IN_DELETE:
	 *	Description: File/directory deleted from within watched directory
	 *	Action	   : if it's a dir delete watch for it and send a message
	 *
	 * IN_DELETE_SELF:
	 *	Description: Watched file/directory was itself deleted
	 *	Action:
	 * 
	 * IN_MODIFY:
	 *	Description:  File was modified
	 *	Action	   :
	 *
	 * IN_MOVE_SELF:
	 *	Description: Watched file/directory was itself moved
	 *	Action:
	 *
	 * IN_MOVED_FROM:
	 *	Description: File moved out of watched directory 
	 * IN_MOVED_FROM:
	 * 	Description: File moved into watched directory
	 * IN_MOVE:
	 *	Description: Shorthand for IN_MOVED_FROM | IN_MOVED_TO
	 * 
	 * IN_CLOSE_WRITE:
	 *	Description: File opened for writing was closed
	 *	Action:
	 *
	 * IN_OPEN:
	 *	Description: File was opened
	 *	Action:
	 */

	uint32_t event_mask = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVE | IN_CLOSE_WRITE;
	int watch_fd;
	char *watched_names[num_dirs]; // path of all the watche dirs 
	init_triggered_names(EREPORT_DIM, 0); //init to NULL	
	struct watched_dir *node = head;
	printf(" ** N: %d dirs have been recognized in %s\n", num_dirs, argv[1]);
	printf(" ** See %s for a complete list of recognized dirs\n", LOGS_FILENAME);
	print_watched_dir(node);
	printf(" ** Adding watches on recognized dirs\n");
	while(node){
		if ((watch_fd = inotify_add_watch(inotify_fd, node->name, event_mask)) < 0) {
                        printf("error adding watch for %s %s\n", node->name, strerror(errno));
      		} else {
			printf(" ** Added %s to watch list on descriptor %d\n", node->name, watch_fd);
                        /* Record the file we're watching on this watch descriptor */
                        /* There should be a check on overflow of the watchednames array */
			watched_names[watch_fd] = (char *) malloc(strlen(node->name)*sizeof(char)+1);
                        strcpy(watched_names[watch_fd], node->name);
                }
		node = node->next;
	}

	char *p;
	ssize_t n;
	stop = 0;
 	while(1 && !stop) {
    		n = read(inotify_fd, input_buffer, BUF_LEN);
    		/* Loop over all events and report them. This is a little tricky
       		   because the event records are not of fixed length
    		*/
		printf("Read %ld bytes from inotify fd\n", (long) n);
    		for (p = input_buffer; p < input_buffer + n;) {
      			event = (struct inotify_event *) p;
			struct inotify_event *inode =(struct inotify_event *)malloc(sizeof(struct inotify_event)+sizeof(char)*event->len);
			copy_inotify_event(inode, event); // deep copy not swallow copy
      			p += sizeof(struct inotify_event) + event->len;
      			/* Display the event */
 			display_inotify_event(event);
			/* Enqueue the event */
			char *path_dir = watched_names[event->wd]; // it already has ending /
			//char *filename = event->name;
			//char *path_filename = (char *)malloc(sizeof(char)*(strlen(path_dir)+event->len)+1);
			char *path_filename = (char *)malloc(sizeof(char)*PATH_MAX);
			//char *path_filename = (char *)malloc(sizeof(char)*(strlen(path_dir)+event->len)+1);
			printf("path_dir=%ld event->len=%d path_filename=%ld\n", strlen(path_dir), event->len, sizeof(path_filename));
			int written_byte = snprintf(path_filename, sizeof(char)*(PATH_MAX), "%s%s", path_dir, event->name);
			printf("written_byte=%d\n", written_byte);
			int index = add_elem_in_triggered_names(path_filename, EREPORT_DIM_REAL);
			printf("INDEX--->%d\n", index);
			/* 
			 * TODO: if index == -1 no more space...realloc
			 *       We need to realloc both
			 *	 EREPORT[] and TRIGGERED_NAMES[]
			 */
			if(index == -1){
				/*
				 * if index == -1 no more space in TRIGGERED_NAMES
				 * we need to increase dim of EREPORT_INC_DIM unit
				 * init to null all new elements and the try to re
				 * add that elem that caused buffer overflow.
				 */
				EREPORT_DIM_REAL += EREPORT_INC_DIM;
				
				//EREPORT = (struct queue_event **)realloc(EREPORT, EREPORT_DIM_REAL*sizeof(struct queue_event *)); 
				//triggered_names = (char **)realloc(triggered_names, EREPORT_DIM_REAL*sizeof(char *));
				//int i;
				//for(i=0; EREPORT[i]; i++);
				//printf("i=%d EREPORT_DIM_REAL=%d\n", , EREPORT_DIM_REAL);
				init_triggered_names(EREPORT_DIM_REAL, EREPORT_DIM_REAL-EREPORT_INC_DIM); //init to NULL	
				init_queues_system(EREPORT_DIM_REAL, EREPORT_DIM_REAL-EREPORT_INC_DIM);
				index = add_elem_in_triggered_names(path_filename, EREPORT_DIM_REAL);
			}
			print_triggered_names(EREPORT_DIM_REAL);
			if(EREPORT[index]==NULL)
				EREPORT[index] = create_queue(path_filename);
			enqueue(EREPORT[index], inode);
			print_queue(EREPORT[index], index);	
		}
	}
   
  	for(int i=1; i<=num_dirs; i++)
		free(watched_names[i]);
  	free_watched_dir(head); 
  	free(root_path);
  	fclose(LOGS);      	
}
