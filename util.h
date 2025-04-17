#ifndef __UTIL_H__
#define __UTIL_H__

#define IS_DIR(x) x == 4 ? 1 : 0

char *FILE_TYPE(int t);
char *get_datetime(void);
time_t get_timestamp(void);

#endif
