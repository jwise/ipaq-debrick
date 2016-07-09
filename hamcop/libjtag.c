#include <stdio.h>
#include <stdlib.h>
#include <linux/ppdev.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "libjtag.h"

int pfd;

void
test_reset()
{
	clockin(1, 0);
	clockin(1, 0);
	clockin(1, 0);
	clockin(1, 0);
	clockin(1, 0);
}

void
capture_dr()
{
	clockin(1, 0);	// Select DR scan
	clockin(0, 0);	// Capture DR
}

void
capture_ir()
{
	clockin(1, 0);	// Select DR scan
	clockin(1, 0);	// Select IR scan
	clockin(0, 0);	// Capture IR
}

unsigned int
clock_bits_out(int bitc)
{
	int bits;
	int i;
	
	bits = 0;
	for (i=0; i<bitc; i++)
	{ 
		clockin(0,0);
		
		bits |= (clockout() << i);
	}
	
	return bits;
}

unsigned int
clock_bits_out_rev(int bitc)
{
	int bits;
	int i;
	
	bits = 0;
	for (i=0; i<bitc; i++)
	{ 
		bits <<= 1;
		bits |= clockout();
		
		clockin(0,0);
	}
	
	return bits;
}

void
set_instr(int instr)
{
	int i;
	static int curinstr = 0xFFFFFF;
	
	if (instr == curinstr)
		return;

	capture_ir();
	
	clockin(0,0);
	for (i=0; i<6; i++)
	{
		clockin(i==5, (instr>>i)&1);
	}
	
	clockin(1, 0);	// Update IR
	clockin(0, 0);	// Run-Test/Idle
}

void
openport(void)
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

void
closeport(void)
{
	if (ioctl(pfd, PPRELEASE) < 0)
	{
		perror("Failed to release /dev/parport0");
		close(pfd);
		exit(0);
	}
	
	close(pfd);
}

void
clockin(int tms, int tdi)
{
	unsigned char data;
	
	tms = tms ? 1 : 0;
	tdi = tdi ? 1 : 0;
	
	data = (1 << PROG) | (0 << TCK) | (tms << TMS) | (tdi << TDI);
	if (ioctl(pfd, PPWDATA, &data) < 0)
	{
		perror("Failed to write parport data");
		closeport();
		exit(0);
	}
	data = (1 << PROG) | (1 << TCK) | (tms << TMS) | (tdi << TDI);
	if (ioctl(pfd, PPWDATA, &data) < 0)
	{
		perror("Failed to write parport data");
		closeport();
		exit(0);
	}
	data = (1 << PROG) | (0 << TCK) | (tms << TMS) | (tdi << TDI);
	if (ioctl(pfd, PPWDATA, &data) < 0)
	{
		perror("Failed to write parport data");
		closeport();
		exit(0);
	}
}

void
clock(void)
{
	unsigned char data;
	
	data = (0 << TCK);
	if (ioctl(pfd, PPWDATA, &data) < 0)
	{
		perror("Failed to write parport data");
		closeport();
		exit(0);
	}
	
	data = (1 << TCK);
	if (ioctl(pfd, PPWDATA, &data) < 0)
	{
		perror("Failed to write parport data");
		closeport();
		exit(0);
	}
}

unsigned char
clockout(void)
{
	unsigned char data;
	
	data = (1 << PROG) | (0 << TCK);		/* Data input on the LEADING EDGE of clock so we can do this! */
	if (ioctl(pfd, PPWDATA, &data) < 0)
	{
		perror("Failed to write parport data");
		closeport();
		exit(0);
	}
	
	if (ioctl(pfd, PPRSTATUS, &data) < 0)
	{
		perror("Failed to read  parport data");
		closeport();
		exit(0);
	}
	
	data ^= 0x80;
	data >>= TDO;
	data &= 1;
	
	return data;
}

void
look_for_pxa()
{
	int i;
	int id;

	set_instr(IDCODE);
	
	capture_dr();
	
	clockin(0, 0);	// Shift DR
	
	id = clock_bits_out_rev(32);
	
	clockin(1, 0);	// exit1
	clockin(1, 0);	// update
	
	clockin(0, 0);	// enter run-test/idle state;
	
	if ((id & 0xFFF00000) != 0x64000000)
	{
		printf("This is not a Samsung chip - Manf %08x. ", id & 0xFFF00000);
		printf("ID: ");
		for (i=0; i<32; i++)
			printf("%d", (id >> i) & 1);
		printf(" (%08X)\n", id);
		closeport();
		exit(0);
	}
	if ((id & 0x000FFFF0) != 0x00013240)
	{
		printf("This is not a HAMCOP - Part %08x. ", id & 0x000FFFF0);
		printf("ID: ");
		for (i=0; i<32; i++)
			printf("%d", (id >> i) & 1);
		printf(" (%08X)\n", id);
		closeport();
		exit(0);
	}
	switch(id & 0xF)
	{
		case 0xb:
			printf("Found HAMCOP on h2210 (ID: %08X)\n", id);
			break;
		default:
			printf("Unsupported HAMCOP stepping %01x. ", id & 0xF);
			printf("ID: ");
			for (i=0; i<32; i++)
				printf("%d", (id >> i) & 1);
			printf(" (%08X)\n", id);
			closeport();
			exit(0);
	}
}
