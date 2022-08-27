#include <fcntl.h>

int main() {
	int fd = open("hola", O_WRONLY | O_CREAT);
	for (int i = 0; i < 0x100; i++) {
		write(fd, &i, 1);
	}
	close(fd);
}