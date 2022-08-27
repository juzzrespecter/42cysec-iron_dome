#include "../inc/irondome.h"

//error hadling could be better with some pritns or using perror

void entropy_file(char* path, int arr[256])
{
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return ;
	int amount = 10;
	unsigned char *save = malloc(1001);

	while (amount > 0) {
		amount = read(fd, save, 1000);
		for (int i = 0; i < amount; i++)
			arr[save[i]] += 1;
	}
	free(save);
	close(fd);
}

void entropy_dir(char* dir_path, int arr[256])
{
	DIR* dir = opendir(dir_path);
	
	if (!dir)
		return ;

	struct dirent *elem = readdir(dir);
	char *join1, *join2;

	while (elem != NULL) {
		//printf("%s\n", elem->d_name);
		//fflush(NULL);
		if (elem->d_type == 4 && strcmp(elem->d_name, ".") && strcmp(elem->d_name, ".."))
		{
			join1 = ft_strjoin(dir_path, "/");
			join2 = ft_strjoin(join1, elem->d_name);
			entropy_dir(join2, arr);
			free(join1);
		}
		if (elem->d_type == 8)
		{
			join1 = ft_strjoin(dir_path, "/");
			join2 = ft_strjoin(join1, elem->d_name);
			entropy_file(join2, arr);
			free(join1);
			free(join2);
		}
		elem = readdir(dir);
	}
	closedir(dir);
	free(dir_path);
}

//recieves path of the dir to find entropy
double entropy(char *path)
{
	int arr[256];

	for (int i = 0; i < 256; i++)
		arr[i] = 0;
	
	entropy_dir(strdup(path), arr);

	long long int len = 0;
	for (int i = 0; i < 256; i++)
		len += arr[i];
	double sum = 0;
	for (int i = 0; i < 256; i++)
	{
		if (arr[i])
		{
			double probability = (double) arr[i] / (double) len;
			sum -= probability * log(probability) / log(256);
		}
	}
	return sum;
}


