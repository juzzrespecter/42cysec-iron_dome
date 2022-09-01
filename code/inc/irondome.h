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
# include <sys/stat.h>
# include <signal.h>

typedef struct inotify_event ievent_t;

# define INIT_CTX(ctx)    memset(&ctx, 0, sizeof(ctx))
# define CLEAR_EVN(ctx)   ctx.n_events = 0
# define RM_FILE_CNT(ctx) ctx.n_files == 0 ? ctx.n_files = 0 : ctx.n_files--

# define EVN_BUF_LEN (sizeof(ievent_t) + NAME_MAX + 1) * 200
# define DIR_ARR_LEN 2
# define NEW 0
# define RM  1

typedef struct {
	int fd;
	char **argv;
	pthread_mutex_t *mutex_write;
	pthread_mutex_t *mutex_sync;
} shared_resources;

typedef struct event_node {
    int   wd;
    char *pathname;
    struct event_node *n;
} event_node_t;

typedef struct monitor_ctx {
    int               fd;       /* inotify file descriptor */
    char            **file_ext; /* list of file extensions to monitor */
    event_node_t     *alst;     /* event list */
    shared_resources *sr;       /* shared context with entropy thread */

    int            n_events;
    int            n_files;
} monitor_ctx_t;

void *fs_monitor(void *);

/* event list funcs. */
event_node_t **add_event(int, char *, event_node_t **);
event_node_t **rm_event(int, event_node_t *, event_node_t **);
void           clean_event_node(int fd, event_node_t *n);

/* fs utils */
int extcmp(char *filename, char **extarr);
void set_pathname(char *pathbuf, char *pathname, char *dirname);
void monitor_logger(int id, char *pathname, monitor_ctx_t *ctx);
void event_logger(monitor_ctx_t *ctx);
int is_invalid_path(char *path);

void	*entropy(void *shared);
char	*ft_strjoin(char const *s1, char const *s2);
int		ends_with(char *str, char e);
char	**arrdup(int len, char **arr);
void	write_to_log(int fd, pthread_mutex_t *mutex, char* message);
int		read_end();
void	end_to_true();

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
