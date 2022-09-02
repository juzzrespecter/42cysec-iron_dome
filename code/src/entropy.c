#include "../inc/irondome.h"

void entropy_file(char* path, int arr[256])
{
	// printf("%s", path);
	// fflush(NULL);
	unsigned char save[1001] = {0};

	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return ;
	//unsigned char *save = malloc(1001);
	//if (!save)
	//{
	//	close(fd);
	//	end_to_true();
	//	return ;
	//}
	int amount = read(fd, save, 1000);

	while (amount > 0) {
		for (int i = 0; i < amount; i++)
			arr[save[i]] += 1;
		amount = read(fd, save, 1000);
	}
	//free(save);
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
	{
		end_to_true();
		return ;
	}

	struct dirent *elem = readdir(dir);
	char *join1, *join2;

	while (elem != NULL && !read_end()) 
	{
		if ((elem->d_type == DT_DIR || elem->d_type == DT_REG) && strcmp(elem->d_name, "pagemap"))
		{
			if (!ends_with(dir_path, '/'))
				join1 = ft_strjoin(dir_path, "/");
			else 
				join1 = strdup(dir_path);
			if (!join1) 
			{
				end_to_true();
				break ;
			}
			join2 = ft_strjoin(join1, elem->d_name);
			if (!join2)
			{
				end_to_true();
				free(join1);
				break ;
			}
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

//recieves path of the dir to find entropy
void *entropy(void *shared_void)
{
	shared_resources *shared = (shared_resources *) shared_void;
	int arr[256];
	double initial_ent;
	double curr_ent;
	double probability;
	int warning_level = 0;
	int counter = 100;

	while (!read_end())
	{
		for (int i = 0; i < 256; i++)
			arr[i] = 0;
		
		entropy_dir((shared->argv)[0], arr);

		if (!read_end())
		{
			long long int len = 0;
			for (int i = 0; i < 256; i++)
				len += arr[i];
			curr_ent = 0;
			for (int i = 0; i < 256; i++)
			{
				if (arr[i])
				{
					probability = (double) arr[i] / (double) len;
					curr_ent -= probability * log(probability) / log(256);
				}
			}
			if (counter == 100)
			{		
				initial_ent = curr_ent;
				counter = 0;
			}
			//50%
			if (((curr_ent - initial_ent)/(1 - initial_ent) > 0.697) && warning_level < 3)
			{
				write_to_log(shared->fd, shared->mutex_write, "[ WARNING ] critical entropy change!!\n");
				warning_level = 3;
			}
			//20% 
			else if (((curr_ent - initial_ent)/(1 - initial_ent) > 0.348) && warning_level < 2)
			{
				write_to_log(shared->fd, shared->mutex_write, "[ entropy ] relevant entropy change!!\n");
				warning_level = 2;
			}
			//10%
			else if (((curr_ent - initial_ent)/(1 - initial_ent) > 0.197) && warning_level < 1)
			{
				write_to_log(shared->fd, shared->mutex_write, "[ entropy ] low entropy change!!\n");
				warning_level = 1;
			}
			counter++;
		}
		pthread_mutex_lock(shared->mutex_sync);
		extern int sync_switch;
		sync_switch = 1;
		pthread_mutex_unlock(shared->mutex_sync);
	}
	return NULL;
}
