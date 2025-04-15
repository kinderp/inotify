#ifndef __MONITOR_H__
#define __MONITOR_H__

struct watched_dir {
        char *name;
        struct watched_dir *next;
};

void push_watched_dir(struct watched_dir **head, char *dir_name);
void print_watched_dir(struct watched_dir *node);

void display_inotify_event(struct inotify_event *i);

#endif
