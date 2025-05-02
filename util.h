#ifndef __UTIL_H__
#define __UTIL_H__

#define IS_DIR(x) x == 4 ? 1 : 0

char *FILE_TYPE(int t);
time_t get_timestamp(void);
void init_ptr_char_v(int dim, int is_realloc);
int add_elem_in_ptr_char_v(char **head, char *elem, int dim);
#endif
