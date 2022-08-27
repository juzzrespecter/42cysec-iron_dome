#include "irondome.h"

static void usage(void)
{
    dprintf(STDERR_FILENO, "usage: ./irondome DIR [FILE_EXT...]");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 2)
		usage();
	//parse for file extensions
	printf("Entropy: %f\n", entropy(argv[1]));
}
