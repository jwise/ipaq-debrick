#include "libjtag.h"

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
		exit(0);
	}
}

void closeport(void)
{
	if (ioctl(pfd, PPRELEASE) < 0)
	{
		perror("Failed to release /dev/parport0");
		close(pfd);
		exit(0);
	}
	
	close(pfd);
}

int main(void)
{
	int pins[] = {TCK, TMS, TDI, PROG};
	char* pinnames[] = {"TCK", "TMS", "TDI", "PROG"};
	unsigned char data;
	int i;
	
	openport();
	for (i = 0; i < (sizeof(pins) / sizeof(pins[0])); i++)
	{
		data = 1 << pins[i];
		printf("%s high...\n", pinnames[i]);
		if (ioctl(pfd, PPWDATA, &data) < 0)
		{
			perror("Failed to write parport data");
			closeport();
			exit(0);
		}
		sleep(2);
	}
	
	printf("Waiting for TDO changes...\n");
	i=-1;
	while(1)
	{
		if (ioctl(pfd, PPRSTATUS, &data) < 0)
		{
			perror("Failed to write parport data");
			closeport();
			exit(0);
		}
		data >>= TDO;
		data &= 1;
		if (data != i)
		{
			i = data;
			printf("TDO change: %s\n", i ? "high" : "low");
		}
		sleep(1);
	} 	
	
	closeport();
	return 0;
}

