#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

int main()
{
	char buf[100] = "hello";
	char rbuf[7];
	int fd = open("/dev/orochy", O_RDWR);
	write(fd, buf, strlen(buf));
	printf("Writing into device\n");
	read(fd, rbuf, 6);
	printf("%s", rbuf);
}

