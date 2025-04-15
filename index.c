#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/inotify.h>

#include "monitor.h"
#include "print.h"
#include "util.h"

extern FILE *LOGS;

void indexing_process(char *current_dir, struct watched_dir **head, int *tot_dirs){
        struct dirent **namelist;
        int num_files = 0;
        int num_dirs = 1; // at least argv[1] must be present
        num_files = scandir(current_dir, &namelist, NULL, alphasort);

        //printf("Start SCANNING %s dir\n", current_dir);
        print_progress(1, 100, "START SCANNING");
        for(int i=0; i<num_files; i++){
                if(strcmp(namelist[i]->d_name, "..") == 0 || strcmp(namelist[i]->d_name, ".") == 0)
                        continue;
                char *rel_path = (char *)malloc(PATH_MAX*sizeof(char));
                strcpy(rel_path, current_dir);
                strcat(rel_path, strcat(namelist[i]->d_name, "/"));
                int is_dir = IS_DIR(namelist[i]->d_type);
                if(is_dir){
                        fprintf(LOGS, "%3d) %10s %s\n", num_dirs, rel_path, FILE_TYPE(is_dir));
                        print_watched_dir(*head);
                        push_watched_dir(head, rel_path);

                        indexing_process(rel_path, head, tot_dirs);
                        print_watched_dir(*head);
                        num_dirs;
                }

        }
        *tot_dirs += num_dirs;
        //printf("End SCANNING %s dir\n", current_dir);
        print_progress(100, 100, "END SCANNING");

}
