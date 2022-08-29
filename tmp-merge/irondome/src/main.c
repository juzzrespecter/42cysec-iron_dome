#include "irondome.h"

static int usage(void)
{
    fprintf(stderr, "usage: ./irondome DIR [FILE_EXT...]");
    return EXIT_FAILURE;
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

    return monitor(argv[1]);
}
