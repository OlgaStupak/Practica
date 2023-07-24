#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../dev.h"
 
int main()
{
        int fd, cmd;
        printf("\nOpening Driver\n");
        fd = open("/dev/orochy", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
	printf("Reading Value from Driver\n");
        ioctl(fd, IOCTL_GET_MSG, &cmd);
 
        printf("Closing Driver\n");
        close(fd);
}
