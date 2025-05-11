#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define N_LEVEL 3
#define N_FILE_MAX 4
#define N_DIR_MAX 3
#define DEBUG 0

#define GEN_RANDOM(max)  rand() % (max + 1)

struct level_data{
	int n_files;
	int n_dirs;
};

struct level_data INPUT[N_LEVEL];

char **LIST_FILES;
char **LIST_DIRS;
void init_data(void);
void build_playground(int level, char * dir);

int main(void){
	
	mkdir("tests", 0700);
	init_data();
	build_playground(N_LEVEL, "tests");

	return 0;
}


void init_data(void){
	int tot_n_dirs  = 0;
	int tot_n_files = 0;
	printf("**** \tBuilding tests begin!\t ****\n\n");
	printf("   Total Levels n. %d - MAX_DIR n. %d - MAX_FILES %d \n\n", N_LEVEL, N_DIR_MAX, N_FILE_MAX);
	srand(time(NULL));
	for(int i=0; i<N_LEVEL; i++){
		int n_dirs = GEN_RANDOM(N_DIR_MAX);
		int n_files = GEN_RANDOM(N_FILE_MAX);
		INPUT[i].n_dirs  = (n_dirs > 0)  ? n_dirs  : 1;
		INPUT[i].n_files = (n_files > 0) ? n_files :1;
		tot_n_dirs  += INPUT[i].n_dirs;
		tot_n_files += INPUT[i].n_files;
		printf("\t  %dL - %dD - %dF **\n", i+1, INPUT[i].n_dirs, INPUT[i].n_files);
	}
	
	LIST_DIRS  = (char **) malloc(tot_n_dirs*sizeof(char *));
	LIST_FILES = (char **) malloc(tot_n_files*sizeof(char *));
	printf("\n   Initialized lists for %d dirs, %d files\t \n\n", tot_n_dirs, tot_n_files);
	
	printf("   Total Levels n. %d - tot_n_dirs n. %d - tot_n_files n. %d \n\n", N_LEVEL, tot_n_dirs, tot_n_files);
	printf("\n**** \tBuilding tests end!\t ****\n");
}



/* It builds a dirs structure to run our tests
 * int level: depth of dir structure (subdirs)
 * char *dir: dir path of current level
 *
 * n of dirs and n. of files for each level is
 * randomly generated (see init_data() )
 *
 *
 *

 ****    Building tests begin!    ****

   Total Levels n. 3 - MAX_DIR n. 3 - MAX_FILES 4

          1L - 1D - 1F **
          2L - 1D - 2F **
          3L - 3D - 2F **

  ****    Building tests end!      ****


	tests
	├── 0d
	│   ├── 0d
	│   │   ├── 0d
	│   │   │   └── 0f
	│   │   ├── 0f
	│   │   └── 1f
	│   ├── 0f
	│   └── 1f
	├── 1d
	│   ├── 0d
	│   │   ├── 0d
	│   │   │   └── 0f
	│   │   ├── 0f
	│   │   └── 1f
	│   ├── 0f
	│   └── 1f
	└── 2d
	    ├── 0d
	    │   ├── 0d
	    │   │   └── 0f
	    │   ├── 0f
	    │   └── 1f
	    ├── 0f
	    └── 1f

	9 directories, 15 files 
 * 
 * Note: level -> [1:10] but INPUT -> [0,9]
 *       that' s why we do i<INPUT[level-1] 
 */
void build_playground(int level, char *dir){
	static int n_dirs  = 0;
	static int n_files = 0;
	if(level == 0)
		return;
	//char path[PATH_MAX];
	//char filename[PATH_MAX];	
	for(int i=0; i<INPUT[level-1].n_dirs; i++){
		char *path = (char *) malloc((strlen(dir)+2)*sizeof(char));
		snprintf(path, PATH_MAX, "%s/%dd", dir, i); 
		mkdir(path, 0700);
		LIST_DIRS[n_dirs] = path;
		n_dirs++;
		#if DEBUG
		printf("level %d, created D: %s\n", level, path);
		#endif
		for(int i=0; i<INPUT[level-1].n_files; i++){
			char *filename = (char *) malloc((strlen(path)+2)*sizeof(char));	
			snprintf(filename, PATH_MAX, "%s/%df", path, i); 
			creat(filename, 0777);
			LIST_FILES[n_files] = filename;
			n_files++;
			#if DEBUG				
			printf("level %d, created F: %s\n", level, filename);
			#endif
		}
		build_playground(level - 1, path);
	}	
}
