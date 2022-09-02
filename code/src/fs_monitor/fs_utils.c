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
    memset(pathbuf, 0, PATH_MAX);
    strncpy(pathbuf, pathname, strlen(pathname) + 1);
    if (strncmp(pathbuf, "/", 2))
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
    static int   evn_log = 0;
    static int   evn_lvl = 0;
    static char *evn_warning[3] = {
        "[ monitor ] detected MODERATE disk usage on system\n",
        "[ monitor ] detected HIGH disk usage on system\n",
        "[ monitor ] [ WARNING ] detected VERY HIGH disk usage on system\n"
    };
    int   n_events = (ctx->n_events < ctx->n_files) ? 0 : ctx->n_events - ctx->n_files;
    if (!n_events || !ctx->n_files)
        return ;
    float p_events = (float)n_events / (float)ctx->n_files;

    printf("{DEBUGGING} what we got: E %d, NE %d, NF %d, P %f\n", n_events, ctx->n_events, ctx->n_files, p_events);
    if (p_events >= 0.15 && p_events < 0.3 && !evn_lvl)
    {
        write_to_log(ctx->sr->fd, ctx->sr->mutex_write, evn_warning[0]);
        evn_lvl = 1;
    }
    if (p_events >= 0.3 && p_events < 0.45 && evn_lvl < 2)
    {
        write_to_log(ctx->sr->fd, ctx->sr->mutex_write, evn_warning[1]);
        evn_lvl = 2;
    }
    if (p_events >= 0.45 && evn_lvl < 3)
    {
        write_to_log(ctx->sr->fd, ctx->sr->mutex_write, evn_warning[2]);
        evn_lvl = 3;
    }
    evn_log++;
    if (evn_log >= 100)
    {
        evn_log = 0;
        evn_lvl = 0;
    }
}

int is_invalid_path(char *path)
{
    static const char *paths[DIR_ARR_LEN] = {"/proc", "/dev"};

    for (int i = 0; i < DIR_ARR_LEN; i++)
    {
        if (!strncmp(path, paths[i], strlen(paths[i]) + 1))
            return 1;
    }
    return 0;
}