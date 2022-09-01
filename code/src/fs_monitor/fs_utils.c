#include "irondome.h"

int extcmp(char *filename, char **extarr)
{
    if (!extarr[1])
        return 0; /* no file extensions selected, monitoring everything */
    char *ext = strrchr(filename, '.');

    if (!ext)
        return 1;
    for (int i = 1; extarr[i]; i++)
    {
        if (!strncmp(ext, extarr[i], strlen(ext) + 1))
            return 0;
    }
    return 1;
}

void set_pathname(char *pathbuf, char *pathname, char *dirname)
{
    printf("{ DEBUGGING } SIZES: (%ld %ld | %d)\n", strlen(pathname), strlen(dirname), PATH_MAX);
    memset(pathbuf, 0, PATH_MAX);
    strncpy(pathbuf, pathname, strlen(pathname) + 1);
    strncat(pathbuf, "/", 2);
    strncat(pathbuf, dirname, strlen(dirname) + 1);
}

void monitor_logger(int id, char *pathname, monitor_ctx_t *ctx)
{
    if (id > 1)
        return ;
    static char *evn_monitor[2] = {
        "[ monitor ] monitoring new directory > ",
        "[ monitor ] stopped monitoring > "
    };
    char msg_buf[PATH_MAX + 42] = {0};

    sprintf(msg_buf, "%s %s\n", evn_monitor[id], pathname);
    write_to_log(ctx->sr->fd, ctx->sr->mutex_write, msg_buf);
}

void event_logger(monitor_ctx_t *ctx)
{
    static char *evn_warning[3] = {
        "[ monitor ] detected moderate disk usage on system",
        "[ monitor ] detected high disk usage on system",
        "[ monitor ] detected very high disk usage on system"
    };
    int   n_events = (ctx->n_events < ctx->n_files) ? 0 : ctx->n_events - ctx->n_files;
    float p_events = n_events / ctx->n_files;

    printf("{DEBUGGING} what we got: %d, %d, %f\n", n_events, ctx->n_files, p_events);
    if (p_events >= 0.3 && p_events < 0.6)
        write_to_log(ctx->sr->fd, ctx->sr->mutex_write, evn_warning[0]);
    if (p_events >= 0.6 && p_events < 0.9)
        write_to_log(ctx->sr->fd, ctx->sr->mutex_write, evn_warning[1]);
    if (p_events >= 0.9)
        write_to_log(ctx->sr->fd, ctx->sr->mutex_write, evn_warning[2]);
}