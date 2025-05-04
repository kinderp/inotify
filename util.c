#ifndef __UTIL_H__
#define __UTIL_H__

#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

extern char **TRIGGERED_NAMES;
extern FILE *LOGS;

char *FILE_TYPE(int t){
        return t == 1 ? "DIR" : "FILE";
}

time_t get_timestamp(void){
	return time(NULL);
}

void print_triggered_names(int dim){
	fprintf(LOGS, "=========TRIGGERED-NAMES=============\n");
	for(int i=0; i<dim; i++)
		fprintf(LOGS, "%d -> %s\n", i, TRIGGERED_NAMES[i]);
	fprintf(LOGS, "=====================================\n");
}

void init_triggered_names(int dim, int is_realloc){
        if(is_realloc){
                TRIGGERED_NAMES = (char **)realloc(TRIGGERED_NAMES, dim*sizeof(char *));
                for(int j=is_realloc; j<dim; j++)
                        TRIGGERED_NAMES[j] = NULL;
                return;
        }

        TRIGGERED_NAMES = (char **)malloc(dim*sizeof(char *)); // path of the filenamesthe triggred an event
        for(int i=0; i<dim; i++)
                TRIGGERED_NAMES[i] = NULL;
}

int add_elem_in_triggered_names(char *elem, int dim){
        int i;
        for(i=0; i<dim && TRIGGERED_NAMES[i]!=NULL; i++){
                if(strcmp(TRIGGERED_NAMES[i], elem)==0)
                        return i;
        }
        // if here we didn't find elem
        if(i<dim && TRIGGERED_NAMES[i] == NULL){
                TRIGGERED_NAMES[i] = elem;
                return i;
        }
        return -1; // no more space
}

#endif
