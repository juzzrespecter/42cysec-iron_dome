#include <fcntl.h>
#include <stdio.h>

int main() {
	int fd = open("hola", O_WRONLY | O_CREAT, 0600);
	for (int i = 0; i < 0x100; i++) {
		write(fd, &i, 1);
	}
	close(fd);
}