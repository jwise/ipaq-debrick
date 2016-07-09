#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include "libjtag.c"

#define EXTEST		0x00
#define SAMPLEPRELOAD	0x01
#define BYPASS		0x1F
#define CLAMP		0x04
#define HIGHZ		0x08
#define IDCODE		0x1E
#define DBGRX		0x02
#define LDIC		0x07
#define SELDCSR		0x09
#define DBGTX		0x10

int main(int argc, char** argv)
{
	int i;
	unsigned long target;
	int hasdat;
	unsigned long data;
	
	if (argc < 2)
	{
		printf("Usage: %s address [data]\n", argv[0]);
		exit(0);
	}
	target = strtoul(argv[1], 0, 0);
	if (argc == 3)
		data = strtoul(argv[2], 0, 0);
	
	openport();
	
	test_reset();		// returns us in reset state
	clockin(0, 0);		// this leaves us in run-test/idle
	look_for_pxa();		// returns us in run-test/idle state
	
	if (argc == 3)
	{
		set_instr(DBGRX);
		printf("Requesting write from PXA... ");
			fflush(stdout);
			sendtxword(0x21);
		printf("done\n");
	} else {
		set_instr(DBGRX);
		printf("Requesting read from PXA... ");
			fflush(stdout);
			sendtxword(0x11);
		printf("done\n");
	}
	
	printf("Sending address to PXA... ");
		fflush(stdout);
		sendtxword(target);
	printf("done\n");
	
	if (argc == 3)
	{
		set_instr(DBGRX);
		printf("Sending data to PXA... ");
			fflush(stdout);
			sendtxword(data);
		printf("done\n");
	} else {
		set_instr(DBGTX);
		printf("Getting data from PXA... ");
			fflush(stdout);
			hasdat = 0;
			while (!hasdat)
				hasdat = gettxword(&data);
		printf("done\n");
	}
	
	if (argc != 3)
		printf("%08x: %08x\n", target, data);
	else
		printf("Wrote %08x to %08x\n", data, target);

	closeport();
	return 0;
}
