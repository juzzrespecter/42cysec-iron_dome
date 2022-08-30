#include "../inc/irondome.h"

//error hadling could be better with some prints or using perror

void entropy_file(char* path, int arr[256])
{
	// printf("%s", path);
	// fflush(NULL);
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return ;
	unsigned char *save = malloc(1001);
	int amount = read(fd, save, 1000);

	while (amount > 0) {
		for (int i = 0; i < amount; i++)
			arr[save[i]] += 1;
		amount = read(fd, save, 1000);
	}
	free(save);
	close(fd);
	// printf("   done\n");
	// fflush(NULL);
}

void entropy_dir(char* dir_path, int arr[256])
{
	// printf("%s\n", dir_path);
	// fflush(NULL);
	DIR* dir = opendir(dir_path);
	
	if (!dir)
		return ;

	struct dirent *elem = readdir(dir);
	char *join1, *join2;

	while (elem != NULL) 
	{
		if ((elem->d_type == DT_DIR || elem->d_type == DT_REG) && strcmp(elem->d_name, "pagemap"))
		{
			if (!ends_with(dir_path, '/'))
				join1 = ft_strjoin(dir_path, "/");
			else 
				join1 = strdup(dir_path);
			join2 = ft_strjoin(join1, elem->d_name);
			if (elem->d_type == DT_DIR && strcmp(elem->d_name, ".") && strcmp(elem->d_name, "..") && strcmp(join2, "/dev") && strcmp(join2, "/proc"))
			{
				entropy_dir(join2, arr);
			}
			if (elem->d_type == DT_REG)
			{
				entropy_file(join2, arr);
			}
			free(join1);
			free(join2);
		}
		elem = readdir(dir);
	}
	closedir(dir);
	// printf("%s   done\n", dir_path);
	// fflush(NULL);
}

//TODO
//solve the case were nothing is there and smt gets created even though might not make sense
//recieves path of the dir to find entropy
void *entropy(void *shared_void)
{
	int arr[256];
	shared_resources *shared = (shared_resources *) shared_void;
	double prev_ent;
	double curr_ent = 0;

	while (1)
	{
		prev_ent = curr_ent;
		for (int i = 0; i < 256; i++)
			arr[i] = 0;
		
		entropy_dir((shared->argv)[0], arr);

		long long int len = 0;
		for (int i = 0; i < 256; i++)
			len += arr[i];
		curr_ent = 0;
		for (int i = 0; i < 256; i++)
		{
			if (arr[i])
			{
				double probability = (double) arr[i] / (double) len;
				curr_ent -= probability * log(probability) / log(256);
			}
		}
		if (prev_ent != 0)
			if (curr_ent - prev_ent != 0)
				write_to_log(shared->fd, shared->mutex_write, "Entropy change!!\n");
	}
	return NULL;
}
