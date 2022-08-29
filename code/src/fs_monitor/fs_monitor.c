#include "irondome.h"

/* poll constants */
static const int nfds = 1;
static const int timeout = -1;

static monitor_ctx_t ctx; /* monitor context global variable */

static void set_pathname(char *pathbuf, char *pathname, char *dirname)
{
    memset(pathbuf, 0, PATH_MAX);
    strncpy(pathbuf, pathname, strlen(pathname) + 1);
    strncat(pathbuf, "/", 2);
    strncat(pathbuf, dirname, strlen(dirname) + 1);
}

static int event_create_dir(struct inotify_event *evn)
{
    event_node_t *n = ctx.alst;
    char  pathbuf[PATH_MAX] = {0};

    printf("[ debug ] a new directory was created\n");
    /* loop through list until event wd matches */
    while (n)
    {
        if (evn->wd == n->wd)
            break;
        n = n->n;
    }
    if (!n)
        return 1;
    set_pathname(pathbuf, n->pathname, evn->name);
    if (!add_event(ctx.fd, pathbuf, &ctx.alst))
	return 1;
    return 0;
}

static int event_rm_dir(struct inotify_event *evn)
{
    event_node_t *n = ctx.alst;

    printf("[ debug ] a directory was deleted\n");
    while (n)
    {
        if (evn->wd == n->wd)
            break;
        n = n->n;
    }
    if (!n)
        return 1;
    rm_event(ctx.fd, n, &ctx.alst);
    return 0;
}

static event_node_t *recursive_dir_access(char *pathname)
{
    DIR *root_st;
    struct dirent *dir_st; /* statically allocated, do not free ! */
    char pathbuf[PATH_MAX + 1];

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
	if (dir_st->d_type == DT_REG)
	    ctx.n_files++;
        if (dir_st->d_type != DT_DIR || !strncmp(pathbuf, "/dev", 5) || !strncmp(pathbuf, "/proc", 5))
            continue;
        printf("[debug] found directory: %s\n", pathbuf);
        if (!add_event(ctx.fd, pathbuf, &ctx.alst))
	{
	    closedir(root_st);
            return NULL;
	}
        recursive_dir_access(pathbuf);
    }
    closedir(root_st);
    if (errno != 0)
    {
        perror("readdir");
        return NULL;
    }
    return ctx.alst;
}

static void DEBUG_print_list(void)
{
    event_node_t *n = ctx.alst;

    for (int i = 0; n != NULL; n = n->n)
    {
	printf("< -------- node {%d} -------- >\n", i);
	printf("wd: %d\n", n->wd);
	printf("path: %s\n\n", n->pathname);
	i++;
    }
}

static void DEBUG_id_event(int wd)
{
    printf("mask type:\n");
    if (wd & IN_ACCESS)        printf("\tIN_ACCESS\n");
    if (wd & IN_ATTRIB)        printf("\tIN_ATTRIB\n");
    if (wd & IN_CLOSE_NOWRITE) printf("\tIN_CLOSE_NOWRITE\n");
    if (wd & IN_CREATE)        printf("\tIN_CREATE\n");
    if (wd & IN_DELETE)        printf("\tIN_DELETE\n");
    if (wd & IN_DELETE_SELF)   printf("\tIN_DELETE_SELF\n");
    if (wd & IN_MODIFY)        printf("\tIN_MODIFY\n");
    if (wd & IN_MOVE_SELF)     printf("\tIN_MOVE_SELF\n");
    if (wd & IN_MOVED_FROM)    printf("\tIN_MOVED_FROM\n");
    if (wd & IN_MOVED_TO)      printf("\tIN_MOVED_TO\n");
    if (wd & IN_OPEN)          printf("\tIN_OPEN\n");
}

static void DEBUG_print_event(struct inotify_event *ptr)
{
    printf("< -------- event --------> \n");
    printf("wd: (%d)\n", ptr->wd);
    printf("mask: (%u)\n", ptr->mask);
    DEBUG_id_event(ptr->wd);
    printf("cookie: (%u)\n", ptr->cookie);
    printf("len: (%u)\n", ptr->len);
    printf("name: (%s)\n\n", ptr->name);
}

static void event_loop(void)
{
    int  evn_len;
    char evn_buf[EVN_BUF_LEN] = {0};
    struct inotify_event *evn;

    while (true)
    {
	printf("[ debug ] read lee de (%d)\n", ctx.fd);
        evn_len = read(ctx.fd, evn_buf, sizeof(evn_buf));
        if (evn_len == -1)
            break;
	
        for (char *ptr = evn_buf; ptr < evn_buf + evn_len;
             ptr += sizeof(struct inotify_event) + evn->len)
        {
            evn = (struct inotify_event *)ptr;
	    DEBUG_print_event(evn);

            /* event is a file creation && file is a directory */
            if (evn->mask & IN_CREATE) /* test if masks are compatible */
            {
		if (evn->mask & IN_ISDIR)
		    event_create_dir(evn);
		else
		    ctx.n_files++;
            }
            if (evn->mask & IN_DELETE)
            {
		if (evn->mask & IN_ISDIR)
		    event_rm_dir(evn);
		else
		    RM_FILE_CNT(ctx);
            }
            if (evn->mask & IN_DELETE_SELF)
	    {
		clean_ctx(&ctx);
		exit(0);
	    }
            if (evn->mask & IN_ACCESS || evn->mask & IN_OPEN)
            {
                /* check here list of file extensions */
                printf("[ debug ] file access in monitored directory\n");
                /* save into montoring structure */
            }
            printf("[ debug ] got event in %s\n", evn->name);
        }
    }
}

int fs_monitor(char *root)
{
    int           nevents;
    struct pollfd fds;

    printf("[ debug ] buf siz: %ld\n", EVN_BUF_LEN);
    INIT_CTX(ctx);
    ctx.fd = inotify_init1(IN_NONBLOCK);
    if (ctx.fd == -1)
    {
        perror("inotify_init1");
        return 1;
    }
    printf("[ debug ] inotify me da (%d)\n", ctx.fd);
    if (!recursive_dir_access(root))
    {
	clean_ctx(&ctx); 
        return 1;
    }
    DEBUG_print_list();
    fds.fd = ctx.fd;
    fds.events = POLLIN;
    while (1)
    {
	printf("[ WAITING IN POLL ]\n");
        nevents = poll(&fds, nfds, timeout);
        if (nevents == -1)
        {
            if (errno == EINTR)
                continue ;
            perror("poll");
            return 1;
        }
        if (fds.revents & POLLIN)
            event_loop();
    }
}
