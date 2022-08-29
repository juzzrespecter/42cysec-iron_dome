#include "irondome.h"

/* add to back of list */
static void add_back(event_node_t **alst, event_node_t *n)
{
    event_node_t *m = *alst;

    if (!m)
    {
        *alst = n;
        return;
    }
    while (m->n)
        m = m->n;
    m->n = n;
}

/* add new event node from directory path name to inotify fd*/
event_node_t **add_event(int fd, char *pathname, event_node_t **alst)
{
    int wd;
    event_node_t *n

        wd = inotify_add_watch(fd, pathname, IN_OPEN | IN_DELETE_SELF | IN_CREATE);
    if (wd == -1)
    {
        perror("inotify_add_watch");
        return NULL;
    }
    n = malloc(sizeof(event_node_t));
    if (!n)
    {
        perror("malloc");
        inotify_rm_watch(fd, wd);
        return NULL;
    }
    n->wd = wd;
    n->pathname = strdup(pathname);
    n->n = NULL;
    add_back(alst, n);
    return alst;
}

static void clean_event_node(int fd, event_node_t *n)
{
    inotify_rm_watch(fd, n->wd);
    free(n->pathname);
    free(n);
}

/* remove event form list */
event_node_t **rm_event(int fd, event_node_t *n, event_node_t **alst)
{
    event_node_t *prev = *alst;
    event_node_t *next = n->n;

    while (prev && prev->n != n)
        prev = prev->n;
    if (!prev)
        return NULL;
    prev->n = next;
    clean_event_node(fd, n);
}

void clean_ctx(int fd, event_node_t **alst)
{
    event_node_t *n = *alst;
    event_node_t *m;

    while (!n->n)
    {
        m = n->n;
        clean_event_node(fd, n);
        n = m;
    }
    free(fd);
}
