#include <stdio.h>	 	/* input-output 	*/
#include <stdlib.h>	 	/* exit() 		*/
#include <string.h>	 	/* strcmp() strcpy() 	*/
#include <unistd.h>	 	/* read() 		*/
#include <sys/inotify.h> 	/* all inotify calls 	*/
#include <sys/stat.h>    	/* stat() 		*/
#include <limits.h>     	/* NAME_MAX, PATH_MAX	*/
#include <dirent.h>     	/* scandir() 		*/

#include <errno.h>		/* strerror(errno)	*/

#define IS_DIR(x) x == 4 ? 1 : 0

char *FILE_TYPE(int t){ 
	return t == 1 ? "DIR" : "FILE";
}

/*
 * 1. I need to define a right dim for inotify input buffer
 *    In this buffer, we're gonna  read triggered events on
 *    watched files/direcotories  
 */

struct watched_dir {
	char *name;
	struct watched_dir *next;
};

void push_watched_dir(struct watched_dir **head, char *dir_name){
	struct watched_dir *new_elem = (struct watched_dir *) malloc (sizeof(struct watched_dir));
	new_elem->name = dir_name;
	new_elem->next = *head;
	*head = new_elem;
	
}

void print_watched_dir(struct watched_dir *node){
	while(node){
		printf("%s\n", node->name);
		node = node->next;
	}
}

void indexing_process(char *current_dir, struct watched_dir **head, int *tot_dirs){
	struct dirent **namelist;
	int num_files = 0;
	int num_dirs = 1; // at least argv[1] must be present 
       	num_files = scandir(current_dir, &namelist, NULL, alphasort);
	
	//char *root_path = (char *)malloc(PATH_MAX*sizeof(char));
	//realpath(argv[1], root_path);
	//strcat(root_path, "/");
	struct stat sb;
	printf("Start SCANNING %s dir\n", current_dir);
	for(int i=0; i<num_files; i++){
		if(strcmp(namelist[i]->d_name, "..") == 0 || strcmp(namelist[i]->d_name, ".") == 0)
			continue;
		char *rel_path = (char *)malloc(PATH_MAX*sizeof(char));
		strcpy(rel_path, current_dir);
		strcat(rel_path, strcat(namelist[i]->d_name, "/"));
		int is_dir = IS_DIR(namelist[i]->d_type);
		if(is_dir){
			printf("%3d) %10s %s\n", num_dirs, rel_path, FILE_TYPE(is_dir));
			print_watched_dir(*head);
			push_watched_dir(head, rel_path);
			
			indexing_process(rel_path, head, tot_dirs);
			print_watched_dir(*head);
			num_dirs;
		}

	}
	*tot_dirs += num_dirs;
	printf("End SCANNING %s dir\n", current_dir);
	
}

void display_inotify_event(struct inotify_event *i){
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[]){

	if (argc < 2 || strcmp(argv[1], "--help") == 0){
        	printf("%s pathname...\n", argv[0]);
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
	struct watched_dir *head = (struct watched_dir *) malloc(sizeof(struct watched_dir));
	head->name = argv[1];
	head->next = NULL;

	int num_files = 0;
	int num_dirs = 1; // at least argv[1] must be present 
       	num_files = scandir(argv[1], &namelist, NULL, alphasort);

	char *root_path = (char *)malloc(PATH_MAX*sizeof(char));
	realpath(argv[1], root_path);
	strcat(root_path, "/");
	struct stat sb;
	indexing_process(root_path, &head, &num_dirs); 
	/*
	printf("Start SCANNING %s dir\n", argv[1]);
	for(int i=0; i<num_files; i++){
		char *rel_path = (char *)malloc(PATH_MAX*sizeof(char));
		strcpy(rel_path, root_path);
		strcat(rel_path, namelist[i]->d_name);
		int is_dir = IS_DIR(namelist[i]->d_type);
		printf("%3d) %10s %s\n", i, rel_path, FILE_TYPE(is_dir));
		if(strcmp(namelist[i]->d_name, "..") == 0 || strcmp(namelist[i]->d_name, ".") == 0)
			continue;
		if(is_dir){
			print_watched_dir(head);
			push_watched_dir(&head, rel_path);
			print_watched_dir(head);
			num_dirs;
		}

	}
	printf("End SCANNING %s dir\n", argv[1]);
	*/


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
	 */

	uint32_t event_mask = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVE;
	int watch_fd;
	char *watched_names[num_dirs];
	struct watched_dir *node = head;
	while(node){
		if ((watch_fd = inotify_add_watch(inotify_fd, node->name, event_mask)) < 0) {
                        printf("error adding watch for %s %s\n", node->name, strerror(errno));
      		} else {
			printf("added %s to watch list on descriptor %d\n", node->name, watch_fd);
                        /* Record the file we're watching on this watch descriptor */
                        /* There should be a check on overflow of the watchednames array */
			watched_names[watch_fd] = (char *) malloc(strlen(node->name)*sizeof(char)+1);
                        strcpy(watched_names[watch_fd], node->name);
                }
		node = node->next;
	}

	char *p;
	ssize_t n;	
 	while(1) {
    		n = read(inotify_fd, input_buffer, BUF_LEN);
    		/* Loop over all events and report them. This is a little tricky
       		   because the event records are not of fixed length
    		*/
		printf("Read %ld bytes from inotify fd\n", (long) n);
    		for (p = input_buffer; p < input_buffer + n;) {
      			event = (struct inotify_event *) p;
      			p += sizeof(struct inotify_event) + event->len;
      			/* Display the event */
 			display_inotify_event(event);
    	}
  }
      	
}
