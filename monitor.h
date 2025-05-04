#ifndef __MONITOR_H__
#define __MONITOR_H__

struct watched_dir {
        char *name;
        struct watched_dir *next;
};

void push_watched_dir(struct watched_dir **head, char *dir_name);
void print_watched_dir(struct watched_dir *node);
void free_watched_dir(struct watched_dir *node);

struct node_ievent {
	struct inotify_event* e;
	struct node_ievent *next;
};

struct queue_event {
	char *path;
	struct node_ievent *head;
	struct node_ievent *tail;
};

struct queue_event* create_queue(char *path);
struct node_ievent* new_node_ievent(struct inotify_event *i);
int queue_is_empty(struct queue_event *q);
void enqueue(struct queue_event *q, struct inotify_event *i);
void dequeue(struct queue_event *q);
void print_queue(struct queue_event* q, int queue_num);
void copy_inotify_event(struct inotify_event *dst, const struct inotify_event *src);
void display_inotify_event(struct inotify_event *i);
void init_queues_system(int dim, int realloc);
#endif
