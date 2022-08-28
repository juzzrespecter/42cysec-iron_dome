#include "irondome.h"

static void set_pathname(char* pathbuf, char* pathname, char* dirname)
{
    memset(pathbuf, 0, PATH_MAX);
    strncpy(pathbuf, pathname, strlen(pathname) + 1);
    strncat(pathbuf, "/", 2);
    strncat(pathbuf, dirname, strlen(dirname) + 1);
}

void event_loop(int fd, int wd)
{
    int i = 0, l;
    char evn_buf[EVN_BUF_LEN] = {0};
    char pathbuf[PATH_MAX] = {0};
    struct inotify_event *event;

    while (i == 0)
    {
        /* read from event file descriptor */
        printf("uh\n");
        l = read(fd, evn_buf, sizeof(evn_buf));
        printf("oh\n");
        if (l == -1)
            break ;
        for (char *ptr = evn_buf; ptr < evn_buf + l;
                ptr += sizeof(struct inotify_event) + event->len)
            {
                event = (struct inotify_event* )ptr;

                if (event->mask & IN_CREATE | IN_ISDIR) /* test if masks are compatible */
                {
                    /* check if new file is a directory, if true then add new watch */
                    printf("[placeholder] new directory created\n");
                    add_event(fd, get_event_pathname(event->name));
                } 
                else if (event->mask & IN_DELETE | IN_ISDIR)
                {
                    printf("[placeholder] directory removed\n");
                }
                else if (event->mask & IN_DELETE_SELF)
                {
                    printf("[placeholder] monitored directory was deleted\n");
                    /* remove watch fd from list */
                } 
                else if (event->mask & IN_ACCESS  || event->mask & IN_OPEN)
                {
                    /* check here list of file extensions */
                    printf("[placeholder] file access in monitored directory\n");
                    /* save into montoring structure */
                }
                printf("got event in %s\n", event->name);
            }
        /* if caught new directory event */
        /* if caught removed directory event */
    }
}

/*
    inotify_event
        int wd;
        uint32_t mask;
        uint32_t cookie;
        uint32_t len;
        char name;
*/

event_node_t* recursive_dir_access(char *pathname, event_node_t *alst)
{
    DIR*           root_st;
    struct dirent* dir_st; /* statically allocated, do not free ! */
    char           pathbuf[PATH_MAX + 1];
    static int     dir_n = 0; /* testing ... */

    root_st = opendir(pathname);
    if (!root_st)
    {
        fprintf(stderr, "opendir: %s\n", strerror(errno));
        return 1;
    }
    errno = 0;
    for (dir_st = readdir(root_st); dir_st != NULL; dir_st = readdir(root_st))
    {
        if (!strncmp(".", dir_st->d_name, 2) || !strncmp("..", dir_st->d_name, 3))
            continue ;
        set_pathname(pathbuf, pathname, dir_st->d_name);            
        if (!is_dir(pathbuf))
            continue ;
        printf("found directory: %s\n", pathbuf);
        recursive_dir_access(pathbuf);
    }
    if (errno != 0)
    {
        fprintf(stderr, "readdir: %s\n", strerror(errno));
        closedir(root_st);
        return 1;
    }
    return 0;
}

int monitor(char* root)
{
    int fd;
    event_node_t* event_lst = NULL;

    fd = inotify_init1(O_NONBLOCK);
    if (fd == -1)
    {
        perror("inotify_init1");
        return 1;
    }
    event_lst = recursive_dir_access(root, &event_lst); /* capture snapshot of current dir. structure, return array of events */
    /* poll */
    while (1)
    {

    }
}