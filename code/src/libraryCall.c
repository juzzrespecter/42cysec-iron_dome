#include "../inc/irondome.h"

void *garbageCollector(char *buffer, FILE *file, char **procSplit)
{
	if (buffer) free(buffer);
	if (file) fclose(file);
	if (procSplit) free_double_ptr(procSplit);

	end_to_true();
	return NULL;
}

int max_proc()
{
	int fd = open("/proc/sys/kernel/pid_max", O_RDONLY);
	if (fd < 0) 
		return 12500;
	char *buf = calloc(20, 1);
	int status = read(fd, buf, 19);
	if (status <= 0) 
		return 12500;
	int res = atoi(buf)/8 + 1;
	free(buf);
	return res;
}

int writtenProc(char proc[], int proc_id)
{
	return ((proc[proc_id/8] & (1 << proc_id % 8)) != 0);
}

void writeProc(char proc[], int proc_id)
{
	proc[proc_id/8] |= (1 << (proc_id % 8));
}

void *libraryCall(void *shared_void)
{
	shared_resources *shared = (shared_resources *) shared_void;
	char *buffer = calloc(10, 1);
	char *aux;
	char **procSplit;
	char *procLine;
	char *join1, *join2;
	int counter;
	char proc[max_proc()];

	for (int i = 0; i < max_proc(); i++)
		proc[i] = 0;
	if (!buffer)
		return (garbageCollector(NULL, NULL, NULL));
	while (!read_end())
	{
		FILE *file = popen("ps -e | awk '{if (NR > 1) {print $1}}'", "r");
		if (!file)
			return (garbageCollector(buffer, NULL, NULL));
		procLine = strdup("");
		if (!procLine)
			return (garbageCollector(buffer, file, NULL));
		while (fgets(buffer, 10, file))
		{
			aux = ft_strjoin(procLine, buffer);
			free(procLine);
			if (!aux)
				return (garbageCollector(buffer, file, NULL));
			procLine = aux;
		}
		fclose(file);
		procSplit = ft_split(procLine, '\n');
		free(procLine);
		if (!procSplit)
			return (garbageCollector(buffer, NULL, NULL));
		counter = 0;
		while (procSplit[counter])
		{
			join1 = ft_strjoin("2>/dev/null ldd /proc/", procSplit[counter]);
			if (!join1)
				return (garbageCollector(buffer, NULL, procSplit));
			join2 = ft_strjoin(join1, "/exe | grep -e crypto -e ssl");
			free(join1);
			if (!join2)
				return (garbageCollector(buffer, NULL, procSplit));
			file = popen(join2, "r");
			free(join2);
			if (!file)
				return (garbageCollector(buffer, NULL, procSplit));
			if (fgets(buffer, 10, file))
			{
				if (!writtenProc(proc, atoi(procSplit[counter])))
				{
					write_to_log(shared->fd, shared->mutex_write, "[ WARNING ] Process with id: ");
					write_to_log(shared->fd, shared->mutex_write, procSplit[counter]);
					write_to_log(shared->fd, shared->mutex_write, " is using cryptographic or ssl libraries\n");
					writeProc(proc, atoi(procSplit[counter]));
				}
			}
			fclose(file);
			counter++;
		}
		for (int i = 0; i < counter; i++)
			free(procSplit[i]);
		free(procSplit);
	}
	free(buffer);
	return NULL;
}
