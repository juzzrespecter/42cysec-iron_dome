#ifndef IRONDOME_H
# define IRONDOME_H
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
# include <sys/inotify.h>
# include <errno.h>
# include <stdbool.h>
# include <limits.h>
# include <poll.h>

# define INIT_CTX(ctx)  memset(&ctx, 0, sizeof(ctx))
# define CLEAR_EVN(ctx) ctx->n_events = 0
# define EVN_BUF_LEN 2000

typedef struct event_node {
    int   wd;
    char *pathname;
    struct event_node *n;
} event_node_t;

typedef struct monitor_ctx {
    int            fd;        /* inotify file descriptor */
    char         **file_ext;  /* list of file extensions to monitor */
    event_node_t  *alst;      /* event list */

    int            n_events;
    int            n_files;
} monitor_ctx_t;

void fs_monitor(char *root);

/* event list funcs. */
event_node_t **add_event(int, char *, event_node_t **);
event_node_t **rm_event(int, event_node_t *, event_node_t **);
void           clean_ctx(monitor_ctx_t *);

double	entropy(char *path);
char	*ft_strjoin(char const *s1, char const *s2);
int		ends_with(char *str, char e);
char	**arrdup(int len, char **arr);

/*
//  * File types
//  */
// #define DT_UNKNOWN       0
// #define DT_FIFO          1
// #define DT_CHR           2
// #define DT_DIR           4
// #define DT_BLK           6
// #define DT_REG           8
// #define DT_LNK          10
// #define DT_SOCK         12
// #define DT_WHT          14

# endif // IRONDOME_H
