#ifndef __UTIL_H__
#define __UTIL_H__

#include<time.h>

char *FILE_TYPE(int t){
        return t == 1 ? "DIR" : "FILE";
}

time_t get_timestamp(void){
	return time(NULL);
}

char *get_datetime(void){
	time_t t;
     	time(&t);
	char *datetime = ctime(&t);
	return datetime;
}

#endif
