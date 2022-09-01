#include "../inc/irondome.h"

int end;
pthread_mutex_t mutex_end;
int sync_switch;

void sig_handler(int signum) 
{
	(void) signum;
	end_to_true();
}

void write_to_log(int fd, pthread_mutex_t *mutex, char* message)
{
	pthread_mutex_lock(mutex);
	int status = dprintf(fd, message);
	if (status < 0)
		end_to_true();
	pthread_mutex_unlock(mutex);
}

int read_end()
{
	pthread_mutex_lock(&mutex_end);
	int aux = end;
	pthread_mutex_unlock(&mutex_end);
	return aux;
}

void end_to_true()
{
	pthread_mutex_lock(&mutex_end);
	end = 1;
	pthread_mutex_unlock(&mutex_end);
}

static int error_printer(char *error)
{
	dprintf(STDERR_FILENO, error);
	return (EXIT_FAILURE);
}

static void create_dir(char *name, int permissions)
{
	int status = mkdir(name, permissions);
	if (status == -1 && errno != EEXIST)
	{
		dprintf(STDERR_FILENO, "couldnt create %s, needed for logger\n", name);
		exit(EXIT_FAILURE);
	}
}

static int create_log(void)
{
	create_dir("/var", 0755);
	create_dir("/var/log", 0755);
	create_dir("/var/log/irondome", 0700);
	int fd = open("/var/log/irondome/irondome.log", O_APPEND | O_CREAT | O_WRONLY, 0600);
	if (fd > 0)
		return fd;
	dprintf(STDERR_FILENO, "couldnt create or access logger file /var/log/irondome/irondome.log\n");
	exit(EXIT_FAILURE);
}

static void free_double_ptr(char **str)
{
	char **copy = str;

	while (str && *str)
	{
		free(*str);
		str++;
	}
	free(copy);
}

static void free_everything(int fd, char **s1, char **s2, pthread_mutex_t *mutex_ptr1, pthread_mutex_t *mutex_ptr2, char *error, int exit_code)
{
	if (fd) close(fd);
	if (s1) free_double_ptr(s1);
	if (s2) free_double_ptr(s2);
	if (mutex_ptr1) pthread_mutex_destroy(mutex_ptr1);
	if (mutex_ptr2) pthread_mutex_destroy(mutex_ptr2);
	if (error) dprintf(STDERR_FILENO, error);
	exit(exit_code);
}

//use pmap to check mem usage

//TODO
//parse for file extensions
//differenciate compresion from encryption through file size

int main(int argc, char **argv)
{
	if (geteuid() != 0)
		return (error_printer("you must be root to execute this binary\n"));
    if (argc < 2)
		return (error_printer("usage: ./irondome DIR [FILE_EXT...]\n"));
	if (!strcmp(argv[1], "/dev") || !strcmp(argv[1], "/proc"))
		return (error_printer("you cant monitor /proc or /dev, this dirs interact directly with the kernel, nobody would want to throw ransomware in them as it may break the system\n"));
	int fd = create_log();

	char **argv_entropy = arrdup(argc - 1, argv);
	if (!argv_entropy) free_everything(fd, NULL, NULL, NULL, NULL, "malloc error\n", 1);

	char **argv_fs = arrdup(argc - 1, argv);
	if (!argv_fs) free_everything(fd, argv_entropy, NULL, NULL, NULL, "malloc error\n", 1);

	pthread_mutex_t mutex_write;
	if (pthread_mutex_init(&mutex_write, NULL) != 0) free_everything(fd, argv_entropy, argv_fs, NULL, NULL, "couldnt create mutex\n", 1);
	
	pthread_mutex_t mutex_sync;
	if (pthread_mutex_init(&mutex_sync, NULL) != 0) free_everything(fd, argv_entropy, argv_fs, &mutex_write, NULL, "couldnt create mutex\n", 1);

	shared_resources shared_entropy;
	shared_entropy.mutex_write = &mutex_write;
	shared_entropy.mutex_sync = &mutex_sync;
	shared_entropy.argv = argv_entropy;
	shared_entropy.fd = fd;

	shared_resources shared_fs;
	shared_fs.mutex_write = &mutex_write;
	shared_fs.mutex_sync = &mutex_sync;
	shared_fs.argv = argv_fs;
	shared_fs.fd = fd;
	(void) shared_fs;

	pthread_t thr_entropy, thr_fs;
	end = 0;
	sync_switch = 0;

	/*daemon(0, 0);*/
	if (pthread_create(&thr_entropy, NULL, &entropy, &shared_entropy) != 0)
		free_everything(fd, argv_entropy, argv_fs, &mutex_write, &mutex_sync, "couldnt create thread", 1);
	if (pthread_create(&thr_fs, NULL, &fs_monitor, &shared_fs) != 0)
	{
		end_to_true();
		free_everything(fd, argv_entropy, argv_fs, &mutex_write, &mutex_sync, "couldnt create thread", 1);
	}

	signal(SIGINT, sig_handler);
	write_to_log(fd, &mutex_write, "Starting monitoring of directory ");
	write_to_log(fd, &mutex_write, argv[1]);
	write_to_log(fd, &mutex_write, "\n\n");
	pthread_join(thr_entropy, NULL);
	pthread_join(thr_fs, NULL);

	write_to_log(fd, &mutex_write, "\n\n");
	free_everything(fd, argv_entropy, argv_fs, &mutex_write, NULL, NULL, 0);
}
