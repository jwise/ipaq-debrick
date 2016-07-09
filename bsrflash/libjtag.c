#include "libjtag.h"

int pfd;

void test_reset()
{
	clockin(1, 0);
	clockin(1, 0);
	clockin(1, 0);
	clockin(1, 0);
	clockin(1, 0);
}

void capture_dr()
{
	clockin(1, 0);	// Select DR scan
	clockin(0, 0);	// Capture DR
}

void capture_ir()
{
	clockin(1, 0);	// Select DR scan
	clockin(1, 0);	// Select IR scan
	clockin(0, 0);	// Capture IR
}

unsigned int clock_bits_out(int bitc)
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

unsigned int clock_bits_out_rev(int bitc)
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

void set_instr(int instr)
{
	int i;
	static int curinstr = 0xFFFFFF;
	
	if (instr == curinstr)
		return;

	capture_ir();
	
	clockin(0,0);
	for (i=0; i<5; i++)
	{
		clockin(i==4, (instr>>i)&1);
	}
	
	clockin(1, 0);	// Update IR
	clockin(0, 0);	// Run-Test/Idle
}

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

void clockin(int tms, int tdi)
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

void clock(void)
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

unsigned char clockout(void)
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

int look_for_pxa()
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
	
	if ((id & 0xFFF00000) != 0xC8000000)
	{
		printf("This is not an Intel chip. ");
		printf("ID: ");
		for (i=0; i<32; i++)
			printf("%d", (id >> i) & 1);
		printf(" (%08X)\n", id);
		closeport();
		exit(0);
	}
	if ((id & 0x000FFFF0) != 0x00026490)
	{
		printf("This is not a PXA250. ");
		printf("ID: ");
		for (i=0; i<32; i++)
			printf("%d", (id >> i) & 1);
		printf(" (%08X)\n", id);
		closeport();
		exit(0);
	}
	switch(id & 0xF)
	{
		case 0x0:
			printf("Intel PXA250 A0\n");
			break;
		case 0x8:
			printf("Intel PXA250 A1\n");
			break;
		case 0x4:
			printf("Intel PXA250 B0\n");
			break;
		case 0xC:
			printf("Intel PXA250 B1\n");
			break;
		case 0x2:
			printf("Intel PXA250 B2\n");
			break;
		case 0xA:
			printf("Intel PXA250 C0\n");
			break;
		case 0x6:
			printf("Intel PXA255 A0\n");
			break;
		default:
			printf("Unsupported PXA stepping. ");
			printf("ID: ");
			for (i=0; i<32; i++)
				printf("%d", (id >> i) & 1);
			printf(" (%08X)\n", id);
			closeport();
			exit(0);
	}
	return id;
};

/* We assume you're already in DBGTX. */
int gettxword(unsigned long* wordptr)
{
	int retval;
	
	capture_dr();
	clockin(0, 0);	// Shift DR
	retval = clockout();	// bit ready
	clockin(0, 0);		// 1
	clockin(0, 0);		// 0
	*wordptr = clock_bits_out(32);
	clockin(0, 0);
	clockin(1, 0);	// exit1
	clockin(1, 0);	// update
	clockin(0, 0);	// enter run-test/idle state;
	
	return retval;
}

void sendtxword(unsigned long txword)
{
	int ready;
	int i;
	
	ready = 1;
	
	while (ready)
	{
		capture_dr();		// prepare to load data
		clockin(0,0);		// enter shift state
		ready = clockout();	// are we ready?
		if (ready)
		{
			clockin(1,0);
			clockin(1,0);
			clockin(0,0);
			fflush(stdout);
			continue;
		}
		clockin(0,0);		// don't set dbg.flush
		clockin(0,0);		// don't set dbg.d 
		for (i = 0; i < 32; i++)
		{
			clockin(0, (txword >> i) & 1);
		}
		clockin(1,1);		// yes, this is valid data (dbg.v)
		clockin(1,0);		// enter update IR state
		clockin(0,0);		// enter run-test/idle state
	}
}

void jstartfastread(unsigned long addr, unsigned long words)
{
	set_instr(DBGRX);
		sendtxword(0x12);
		sendtxword(addr);
		sendtxword(words);
	set_instr(DBGTX);
}

void jstartfastwrite()
{
	set_instr(DBGRX);
		sendtxword(0x28);
}

void jstartfastwriteh()
{
	set_instr(DBGRX);
		sendtxword(0x27);
}

void jstarth1910write()
{
	set_instr(DBGRX);
		sendtxword(0x30);
}

void jstarth3900write()
{
	set_instr(DBGRX);
		sendtxword(0x31);
}

unsigned long jfastread()
{
	unsigned long hasdat, data;
	
	hasdat = 0;
	while (!hasdat)
		hasdat = gettxword(&data);
	
	return data;
}

unsigned long jread(unsigned long addr)
{
	unsigned long hasdat, data;
	
	set_instr(DBGRX);
		sendtxword(0x11);
		sendtxword(addr);

	set_instr(DBGTX);
		hasdat = 0;
		while (!hasdat)
			hasdat = gettxword(&data);
	
	return data;
}

unsigned long jreadh(unsigned long addr)
{
	unsigned long hasdat, data;
	
	set_instr(DBGRX);
		sendtxword(0x10);
		sendtxword(addr);

	set_instr(DBGTX);
		hasdat = 0;
		while (!hasdat)
			hasdat = gettxword(&data);
	
	return data;
}

int jcheck()
{
	unsigned long hasdat, data;
	
	set_instr(DBGRX);
		sendtxword(0xFE);

	set_instr(DBGTX);
	hasdat = 0;
	while (!hasdat)
		hasdat = gettxword(&data);
	
	if (data != 0x0FFFFFFE)
		return data;
	
	return 0;
}

unsigned long jwrite(unsigned long addr, unsigned long value)
{
	unsigned long data = 0;
	
	set_instr(DBGRX);
		sendtxword(0x21);

	sendtxword(addr);
	sendtxword(value);

	return data;
}

unsigned long jwriteh(unsigned long addr, unsigned long value)
{
	unsigned long hasdat, data;
	
	set_instr(DBGRX);
		sendtxword(0x20);

	sendtxword(addr);
	sendtxword(value);

	set_instr(DBGTX);
	hasdat = 0;
	while (!hasdat)
		hasdat = gettxword(&data);
	
	return data;
}

unsigned long jfastwrite(unsigned long addr, unsigned long value)
{
	set_instr(DBGRX);
		sendtxword(addr);
		sendtxword(value);
	
	return 0x0;
}
