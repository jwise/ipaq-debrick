#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ppdev.h>
#include <stdio.h>
#include <stdlib.h>

int pfd;

void openport(void)
{
	pfd = open("/dev/parport0", O_RDWR);
	if (pfd < 0)
	{
		perror("Failed to open /dev/parport0");
		exit(0);
	}
	
	if ((ioctl(pfd, PPEXCL) < 0) || (ioctl(pfd, PPCLAIM) < 0))
	{
		perror("Failed to lock /dev/parport0");
		close(pfd);
		exit(1);
	}
}

void closeport(void)
{
	if (ioctl(pfd, PPRELEASE) < 0)
	{
		perror("Failed to release /dev/parport0");
		close(pfd);
		exit(1);
	}
	
	close(pfd);
}

int main(int argc, char** argv)
{
	unsigned char data;
	
	if ((argc != 2) || argv[1][1] != '\0' || (argv[1][0] != '0' && argv[1][0] != '1'))
	{
		printf("usage: %s [1|0]\n", argv[0]);
		return 1;
	}
	
	openport();
	data = argv[1][0] - '0' ? 0xFF : 0x00;;
	if (ioctl(pfd, PPWCONTROL, &data) < 0)
	{
		perror("Failed to write parport data");
		closeport();
		exit(1);
	}

	closeport();
	return 0;
}

