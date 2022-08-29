#include "irondome.h"

/* need to keep track of number of files in monitored directories */

/* poll constants */
static const int nfds = 1;
static const int timeout = -1;

static int is_dir(char *filename)
{
    struct stat f_info;

    memset(&f_info, 0, sizeof(f_info));
    if (stat(filename, &f_info) == -1)
    {
        fprintf(stderr, "stat: %s\n", strerror(errno));
        errno = 0;
        return 0;
    }
    return S_ISDIR(f_info.st_mode);
}

static void set_pathname(char *pathbuf, char *pathname, char *dirname)
{
    memset(pathbuf, 0, PATH_MAX);
    strncpy(pathbuf, pathname, strlen(pathname) + 1);
    strncat(pathbuf, "/", 2);
    strncat(pathbuf, dirname, strlen(dirname) + 1);
}

static int event_create_dir(int fd, struct inotify_event *evn, event_node_t **alst)
{
    event_node_t *n = *alst;
    char *path = NULL;
    char pathbuf[PATH_MAX] = {0};

    /* loop through list until event wd matches */
    while (n)
    {
        if (evn->wd == n->wd)
        {
            path = n->pathname;
            break;
        }
        n = n->n;
    }
    if (!n)
        return 1;
    set_pathname(pathbuf, n->pathname, evn->name);
    add_event(fd, pathbuf, alst);
    return 0;
}

static int event_rm_dir(int fd, struct inotify_event *evn, event_node_t **alst)
{
    event_node_t *n = *alst;

    while (n)
    {
        if (evn->wd == n->wd)
            break;
        n = n->n;
    }
    if (!n)
        return 1;
    rm_event(fd, n, alst);
    return 0;
}

void event_loop(int fd, event_node_t **alst)
{
    int i = 0, evn_len;
    char evn_buf[EVN_BUF_LEN] = {0};
    struct inotify_event *evn;

    while (i == 0)
    {
        evn_len = read(fd, evn_buf, sizeof(evn_buf));
        if (evn_len == -1)
            break;
        for (char *ptr = evn_buf; ptr < evn_buf + evn_len;
             ptr += sizeof(struct inotify_event) + evn->len)
        {
            evn = (struct inotify_event *)ptr;

            /* event is a file creation && file is a directory */
            if (evn->mask & IN_CREATE && env->mask & IN_ISDIR) /* test if masks are compatible */
            {
                printf("[DEBUG] new directory created\n");
                event_create_dir(fd, alst);
            }
            else if (evn->mask & IN_DELETE && evn->mask & IN_ISDIR)
            {
                printf("[DEBUG] directory has been removed\n");
                event_rm_dir(fd, evn, alst);
            }
            else if (evn->mask & IN_DELETE_SELF)
            {
                printf("[DEBUG] monitored directory was deleted\n");
                /* clean exit and return */
                return ;
            }
            else if (evn->mask & IN_ACCESS || evn->mask & IN_OPEN)
            {
                /* check here list of file extensions */
                printf("[DEBUG] file access in monitored directory\n");
                /* save into montoring structure */
            }
            printf("got event in %s\n", evn->name);
        }
    }
}

static event_node_t *recursive_dir_access(int fd, char *pathname, event_node_t *alst)
{
    DIR *root_st;
    struct dirent *dir_st; /* statically allocated, do not free ! */
    char pathbuf[PATH_MAX + 1];
    static int dir_n = 0; /* testing ... */

    dir_n++;
    root_st = opendir(pathname);
    if (!root_st)
    {
        perror("opendir");
        return NULL;
    }
    errno = 0;
    for (dir_st = readdir(root_st); dir_st != NULL; dir_st = readdir(root_st))
    {
        if (!strncmp(".", dir_st->d_name, 2) || !strncmp("..", dir_st->d_name, 3))
            continue;
        set_pathname(pathbuf, pathname, dir_st->d_name);
        if (!is_dir(pathbuf) || !strncmp(pathbuf, "/dev", 5) || !strncmp(pathbuf, "/proc", 5))
            continue;
        printf("[debug] found directory: %s\n", pathbuf);
        if (!add_event(fd, pathbuf, alst))
            return NULL;
        recursive_dir_access(fd, pathbuf, alst);
    }
    if (errno != 0)
    {
        perror("readdir");
        closedir(root_st);
        return NULL;
    }
    return alst;
}

int fs_monitor(char *root)
{
    int fd, nevents;
    event_node_t *event_lst = NULL;
    struct pollfd fds;

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1)
    {
        perror("inotify_init1");
        return 1;
    }
    event_lst = recursive_dir_access(root, &event_lst); /* capture snapshot of current dir. structure, return array of events */
    if (!event_lst)
        return 1;
    fds.fd = fd;
    fds.events = POLLIN;
    while (1)
    {
        nevents = poll(fds, nfds, timeout);
        if (nevents == -1)
        {
            if (errno == EINTR)
                continue ;
            perror("poll")
            return 1;
        }
        if (fds.revents & POLLIN)
            event_loop(fd, event_lst);
    }
}