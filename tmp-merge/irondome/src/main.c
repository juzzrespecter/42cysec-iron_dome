#include "irondome.h"

static int usage(void)
{
    fprintf(stderr, "usage: ./irondome DIR [FILE_EXT...]");
    return EXIT_FAILURE;
}

static int is_dir(char* filename)
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

int main(int argc, char *argv[])
{
    if (argc < 2)
	    usage();
    if (getuid())
    {
	    fprintf(stderr, "this user does not have permission to execute\n");
	    return EXIT_FAILURE;
    }
}


/* add new inotify_watch events when a new directory is created */
/* remove inotify_watch events when a directory is deleted */
/* what happens when root directory is erased?? */
/* watch file descriptors should be nnon blocking ?? */