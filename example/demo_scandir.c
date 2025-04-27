/*
 * The program below prints a list of the files in argv[1] 
 * directory in reverse order.
 *
 * scandir(): https://man7.org/linux/man-pages/man3/scandir.3.html
 * dirent.h : https://man7.org/linux/man-pages/man0/dirent.h.0p.html
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	if(argc < 2){
		printf("Usage: demo_scandir <dir>\n");
		exit(1);
	}

	struct dirent **namelist;
        int n;

        n = scandir(argv[1], &namelist, NULL, alphasort);
       	if (n == -1) {
		printf("Can't scan dir %s\n", argv[1]);
              	exit(1);
        }

        while (n--) {
        	printf("%s \t %d\n", namelist[n]->d_name, namelist[n]->d_type);
               	free(namelist[n]);
        }
        free(namelist);	

	return 0;
}
