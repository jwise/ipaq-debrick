/* Parallel port device code */
#include "gtag.h"
#include "ppdev.h"
#include <gnome.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/ppdev.h>

static int pfd = -1;

void parallel_ppdev_writedata(unsigned char data)
{
	if (pfd < 0)
	{
		g_warning("Parport not initialized!");
		return;
	}
	
	if (ioctl(pfd, PPWDATA, &data) < 0)
		perror("ppdev write");
}

unsigned char parallel_ppdev_readstatus()
{
	unsigned char data;
	if (pfd < 0)
	{
		g_warning("Parport not initialized!");
		return;
	}
	
	if (ioctl(pfd, PPRSTATUS, &data) < 0)
		perror("ppdev read");
	return data;
}

void parallel_ppdev_open(int ppn)
{
	char devname[] = "/dev/parport?";
	devname[12] = ppn + '0';
	pfd = open(devname, O_RDWR);
	if (pfd < 0)
	{
		perror("ppdev open");
		return;
	}
	if ((ioctl(pfd, PPEXCL) < 0) || (ioctl(pfd, PPCLAIM) < 0))
	{
		perror("ppdev lock");
		close(pfd);
		pfd = -1;
		return;
	}
}

void parallel_ppdev_close()
{
	if (pfd < 0)
	{
		g_warning("Parport not open!");
		return;
	}
	
	if (ioctl(pfd, PPRELEASE) < 0)
		perror("ppdev release");
	close(pfd);
	pfd = -1;
}
