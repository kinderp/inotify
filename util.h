#ifndef __UTIL_H__
#define __UTIL_H__

#define IS_DIR(x) x == 4 ? 1 : 0

char *FILE_TYPE(int t);
time_t get_timestamp(void);
void print_triggered_names(int dim);
void init_triggered_names(int dim, int is_realloc);
int add_elem_in_triggered_names(char *elem, int dim);
#endif
