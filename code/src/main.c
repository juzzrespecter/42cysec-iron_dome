#include "../inc/irondome.h"

static void not_that_dir(void)
{
	dprintf(STDERR_FILENO, "you cant monitor /proc or /dev, this dirs interact directly with the kernel, nobody would want to throw ransomware in them as it may break the system\n");
    exit(EXIT_FAILURE);
}

static void must_be_root(void)
{    
	dprintf(STDERR_FILENO, "you must be root to execute this binary\n");
    exit(EXIT_FAILURE);
}

static void usage(void)
{
    dprintf(STDERR_FILENO, "usage: ./irondome DIR [FILE_EXT...]\n");
    exit(EXIT_FAILURE);
}
//use pmap to check mem usage

//TODO
//parse for file extensions
//differenciate compresion from encryption through file size

int main(int argc, char **argv)
{
	if (geteuid() != 0)
		must_be_root();
    if (argc < 2)
		usage();
	if (!strcmp(argv[1], "/dev") || !strcmp(argv[1], "/proc"))
		not_that_dir();
	//daemon(0, 0);
	//sleep(10);
	char **hola = arrdup(argc - 1, argv);
	char **adios = arrdup(argc - 1, argv);
	(void) hola;
	(void) adios;
	fs_monitor(argv[1]);
}
