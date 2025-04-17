#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "monitor.h"
#include "print.h"
#include "util.h"

extern FILE *LOGS;

void indexing_process(char *current_dir, struct watched_dir **head, int *tot_dirs){
        struct dirent **namelist;
        int num_files = 0;
        static int num_dirs = 0;
        num_files = scandir(current_dir, &namelist, NULL, alphasort);

	char message[1000];
	snprintf(message, sizeof(message), "Found %d files in %s", num_files, current_dir);
        print_progress(0, 100, message);
	sleep(1);
	int i;
        for(i=0; i<num_files; i++){
                if(strcmp(namelist[i]->d_name, "..") == 0 || strcmp(namelist[i]->d_name, ".") == 0)
                        continue;
                char *rel_path = (char *)malloc(PATH_MAX*sizeof(char));
                strcpy(rel_path, current_dir);
                strcat(rel_path, strcat(namelist[i]->d_name, "/"));
		snprintf(message, sizeof(message), "Processing  %s in %s dir", rel_path, current_dir);
		float status = ((float)i/num_files)*100;
        	print_progress(status == 0.0 ? 100 : status, 100, message);
		sleep(1);
                int is_dir = IS_DIR(namelist[i]->d_type);
		int n_byte = 0;
                if(is_dir){
                        n_byte = fprintf(LOGS, "%d\t %30s %s\n", num_dirs, rel_path, FILE_TYPE(is_dir));
                        //print_watched_dir(*head);
                        push_watched_dir(head, rel_path);
			snprintf(message, sizeof(message), "Processed %d files in %s", i+1, current_dir);
        		print_progress(100, 100, message);
			sleep(1);
			num_dirs++;
                        indexing_process(rel_path, head, tot_dirs);
                        //print_watched_dir(*head);
			snprintf(message, sizeof(message), "Found %d files in %s", num_files, current_dir);
        		print_progress(0, 100, message);
			sleep(1);
                }	
        }
	fflush(LOGS);
	snprintf(message, sizeof(message), "Processed %d files in %s", num_files, current_dir);
        print_progress(100, 100, message);
	sleep(1);
	*tot_dirs = num_dirs;

}
