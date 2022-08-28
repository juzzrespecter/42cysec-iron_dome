#include "irondome.h"

static void add_back(event_node_t **alst, event_node_t *n)
{

}

event_node_t** add_event(int fd, char *pathname, event_node_t **alst)
{
    int           wd;
    event_node_t* n

    wd = inotify_add_watch(fd, pathname, IN_OPEN | IN_DELETE_SELF | IN_CREATE);
    if (wd == -1)
        return NULL;
    n = malloc(sizeof(event_node_t));
    if (!n)
    {
        inotify_rm_watch(fd, wd);
        return NULL;
    }
    n->wd = wd;
    n->n = NULL;
    add_back(alst, n);
    return alst;
}

/* remove event form list */
event_node_t** rm_event(event_node_t *n)
{
    event_node_t* prev;
    event_node_t* next;
}

void clean_event_list(event_node_t **alst)
{
    while()
}

