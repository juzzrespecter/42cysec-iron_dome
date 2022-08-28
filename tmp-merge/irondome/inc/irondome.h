#ifndef IRONDOME_H
# define IRONDOME_H
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <sys/inotify.h>
# include <dirent.h>
# include <sys/stat.h>
# include <string.h>
# include <errno.h>
# include <stdbool.h>
# include <limits.h>

# define EVN_BUF_LEN 2000

typedef struct event_node {
    int wd;
    struct event_node *n;
} event_node_t;

void event_loop(int wd); /* tmp */

event_node_t** add_event(int fd, char *pathname, event_node_t **alst);
static void add_back(event_node_t **alst, event_node_t *n);
void clean_event_list(event_node_t **alst);

# endif // IRONDOME_H
