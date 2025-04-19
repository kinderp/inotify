#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/inotify.h>

#include"monitor.h"

extern FILE* LOGS;
extern struct queue_event **EREPORT;

void push_watched_dir(struct watched_dir **head, char *dir_name){
        struct watched_dir *new_elem = (struct watched_dir *) malloc (sizeof(struct watched_dir));
        new_elem->name = dir_name;
        new_elem->next = *head;
        *head = new_elem;

}

void print_watched_dir(struct watched_dir *node){
        while(node){
                fprintf(LOGS, "%s\n", node->name);
                node = node->next;
        }
	fflush(LOGS);
}

void free_watched_dir(struct watched_dir *node){
	struct watched_dir *tmp;
	while(node){
		tmp = node->next;
		free(node);
		node = tmp;
	}
}

void copy_inotify_event(struct inotify_event *dst, const struct inotify_event *src){
	dst->wd = src->wd;
	dst->mask = src->mask;
	dst->cookie = src->cookie;
	dst->len = src->len;
	strcpy(dst->name, src->name);	
}

void display_inotify_event(struct inotify_event *i){
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

// queue
struct queue_event* create_queue(void){
	struct queue_event *q = (struct queue_event *)malloc(sizeof(struct queue_event));
	q->head = q->tail = NULL;
	return q;
}

// node in a queue
struct node_ievent* new_node_ievent(struct inotify_event *i){
	struct node_ievent *node = (struct node_ievent *)malloc(sizeof(struct node_ievent));
	node->e = i;
	node->next = NULL;
	return node;
}

int queue_is_empty(struct queue_event *q){
	return q->tail == NULL;
}

void enqueue(struct queue_event *q, struct inotify_event *i){
	struct node_ievent *node = new_node_ievent(i);
	if(queue_is_empty(q)){
		q->tail = node;
		q->head = node;
		print_queue(q, i->name, -1);
		return;
	}
	q->tail->next = node;
	q->tail = node;
	print_queue(q, i->name, -1);
}

void dequeue(struct queue_event *q){
	if(queue_is_empty(q))
		return;
	struct node_ievent *i = q->head; // let's save it, we need to free at the end 
	q->head = q->head->next;
	if(q->head == NULL) q->tail == NULL;
	free(i);
	//TODO: print_queue() here!	
}

void print_queue(struct queue_event *q, char *queue_name, int queue_num){
	if(queue_is_empty(q))
		return;
	fprintf(LOGS, "==========PRINT_QUEUE==========\n");
	fprintf(LOGS, "Queue Name: %s, Queue Number: %d\n", queue_name, queue_num);
	fprintf(LOGS, "Head: %p, Tail: %p\n", q->head, q->tail);
	struct node_ievent *node = q->head;
	while(node != NULL){
		char *event;
    		if (node->e->mask & IN_ACCESS)        event = "IN_ACCESS ";
    		if (node->e->mask & IN_ATTRIB)        event = "IN_ATTRIB ";
    		if (node->e->mask & IN_CLOSE_NOWRITE) event = "IN_CLOSE_NOWRITE ";
    		if (node->e->mask & IN_CLOSE_WRITE)   event = "IN_CLOSE_WRITE ";
    		if (node->e->mask & IN_CREATE)        event = "IN_CREATE ";
    		if (node->e->mask & IN_DELETE)        event = "IN_DELETE ";
    		if (node->e->mask & IN_DELETE_SELF)   event = "IN_DELETE_SELF ";
    		if (node->e->mask & IN_IGNORED)       event = "IN_IGNORED ";
    		if (node->e->mask & IN_ISDIR)         event = "IN_ISDIR ";
    		if (node->e->mask & IN_MODIFY)        event = "IN_MODIFY ";
    		if (node->e->mask & IN_MOVE_SELF)     event = "IN_MOVE_SELF ";
    		if (node->e->mask & IN_MOVED_FROM)    event = "IN_MOVED_FROM ";
    		if (node->e->mask & IN_MOVED_TO)      event = "IN_MOVED_TO ";
    		if (node->e->mask & IN_OPEN)          event = "IN_OPEN ";
    		if (node->e->mask & IN_Q_OVERFLOW)    event = "IN_Q_OVERFLOW ";
    		if (node->e->mask & IN_UNMOUNT)       event = "IN_UNMOUNT ";
 
		fprintf(LOGS, "node=%14p noer->next=%16p node->e=%p wd=%2d name=%s mask=%d event=%17s cookie=%4d\n", node, node->next, node->e, node->e->wd, node->e->name, node->e->mask, event, node->e->cookie);
		fflush(LOGS);

		node = node->next;
	}
	fprintf(LOGS, "===============================\n\n");
	fflush(LOGS);
}

void init_queues_system(int dim, int is_realloc){
	if(is_realloc){
		EREPORT = (struct queue_event **)realloc(EREPORT, dim*sizeof(struct queue_event *));
 		for(int j=is_realloc; j<dim; j++){
                	EREPORT[j] = NULL;
                }
		return;
	}
        EREPORT = (struct queue_event **)malloc(dim*sizeof(struct queue_event *));
        for(int i=0; i<dim; i++)
                EREPORT[i] = NULL;
}
