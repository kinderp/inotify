#ifndef __UTIL_H__
#define __UTIL_H__

#include<time.h>
#include<string.h>
#include<stdlib.h>

extern char **TRIGGERED_NAMES;

char *FILE_TYPE(int t){
        return t == 1 ? "DIR" : "FILE";
}

time_t get_timestamp(void){
	return time(NULL);
}

void init_ptr_char_v(int dim, int is_realloc){
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

int add_elem_in_ptr_char_v(char **head, char *elem, int dim){
        int i;
        for(i=0; head[i]!=NULL && i<dim; i++){
                if(strcmp(head[i], elem)==0)
                        return i;
        }
        // if here we didn't find elem
        if(i<dim && head[i] == NULL){
                head[i] = elem;
                return i;
        }
        return -1; // no more space
}

#endif
