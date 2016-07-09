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

#define BASE_ADDR 0x00000000

int main(int argc, char** argv)
{
	unsigned long i;
	int fd;
	int parity;
	int lines;
	
	printf("FastJTAG/PXA\n");
	printf("PXA25x mini icache bootloader\n");
	
	if (argc < 2)
	{
		printf("Usage: %s filename\n", argv[0]);
		printf("Loads a file into the PXA250's mini instruction cache, then boots the system from it.\n");
		exit(0);
	}
	
	openport();
	
	test_reset();		// returns us in reset state
	clockin(0, 0);		// this leaves us in run-test/idle
	look_for_pxa();		// returns us in run-test/idle state
	
	printf("Assert reset now (press enter when asserted)... ");
		fflush(stdout);
		while (getchar() != '\n');
	
	printf("Setting hold_rst and entering halt mode... ");
		fflush(stdout);
		set_instr(SELDCSR);
		capture_dr();
		clockin(0,0);	// Enter shift state
		clockin(0,0);	// Bit discarded
		clockin(0,1);	// hold_rst enabled
		clockin(0,0);	// No external debug break
		clockin(0,0);	// Trace buffer disable
		clockin(0,0);	// Trace buffer mode (wrap around)
		clockin(0,0);	// Method of Entry: Unset
		clockin(0,0);	// Method of Entry: Unset
		clockin(0,0);	// Method of entry: Unset
		clockin(0,0);	// Sticky abort unset
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// Trap reset off
		clockin(0,0);	// Trap undefined instruction off
		clockin(0,0);	// Trap software interrupt off
		clockin(0,0);	// Trap prefetch abort off
		clockin(0,0);	// Trap data abort off
		clockin(0,0);	// undefined
		clockin(0,0);	// Trap IRQ off
		clockin(0,0);	// Trap FIQ off
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,0);	// undefined
		clockin(0,1);	// Halt mode on
		clockin(0,0);	// Global enable read-only, so it doesn't matter.
		clockin(1,0);	// Ignored bit; Exit1-DR
		clockin(1,0);	// Update DR
		clockin(0,0);	// Run-test/idle state
	printf("done\n");
	
	printf("Deassert reset now (press enter when deasserted)... ");
		fflush(stdout);
		while (getchar() != '\n');
	
	printf("Waiting 2100 TCKs... ");
		fflush(stdout);
		for (i=0; i < 2100; i++)
			clockin(0,0);
	printf("done\n");
	
	printf("Invalidating mini instruction cache... ");
		fflush(stdout);
		set_instr(LDIC);	// returns us in run-test/idle state, but with LDIC in the IR.
		capture_dr();		// prepare to load data!
		clockin(0,0);		// enter shift state
		clockin(0,1);		// invalidate mini IC 1
		clockin(0,0);		// invalidate mini IC 2
		clockin(0,0);		// invalidate mini IC 3
		clockin(0,0);		// pad
		clockin(0,0);		// pad
		clockin(0,0);		// pad
		for (i=5; i < 32; i++) clockin(i == 31, 0);
					// finish packet
		clockin(1,0);		// enter update DR state
		clockin(0,0);		// enter run-test/idle state
	printf("done\n");
	
	for (i=0; i<20; i++)
		clockin(0,0);	// let pxa finish its stuff
	
	printf("Opening %s... ", argv[1]);
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		fflush(stdout);
		perror("failed");
		goto signalcplt;
	}
	printf("done\n");
	
	parity = 0;
	
	for (lines = 0; lines < 2048; lines += 4*8)
	{
		int bit;
		unsigned long data;
		
		printf("Writing to mini instruction cache (mapping at 0x%08X)...     \b\b\b\b", BASE_ADDR + lines);
		fflush(stdout);
		
		capture_dr();	// prepare to load data
		clockin(0,0);	// enter shift state
		clockin(0,1);	// write to mini IC
		clockin(0,1);	// write to mini IC
		clockin(0,0);	// write to mini IC
		clockin(0,0);	// pad
		clockin(0,0);	// pad
		clockin(0,0);	// pad
		data = BASE_ADDR + lines;
		for (bit = 5; bit < 32; bit++)
			clockin(bit == 31, (data >> bit) & 1);

		clockin(1,0);	// enter update DR state
		clockin(0,0);	// enter run-test/idle state
		
		for (i = 0; i < 8; i++)
		{
			if (read(fd, &data, 4) < 4)
			{
				perror("failed");
				closeport();
				close(fd);
				exit(0);
			}
			
			parity = 0;
			
			capture_dr();		// prepare to load data
			clockin(0,0);		// enter shift state
			for (bit = 0; bit < 32; bit++)
			{
				clockin(0, (data >> bit) & 1);
				if ((data >> bit) & 1)
					parity = !parity;
			}
			clockin(1,parity);	// write parity bit, finish up

			clockin(1,0);		// enter update DR state
			clockin(0,0);		// enter run-test/idle state
		}
		printf("done\r");
	}
	for (i=0; i<20; i++)
		clockin(0,0);	// let pxa finish its stuff
	printf("\n");
	
signalcplt:
	
	printf("Booting... ");
		fflush(stdout);
		set_instr(SELDCSR);
		capture_dr();
		clockin(0,0);
		clockin(0,0);
		clockin(0,0);	//hold_rst bit
		clockin(0,0);
		for (i=3; i < 35; i++)
			clockin(i == 35, 0);
		clockin(1,0);
		clockin(0,0);
	printf("done\n");
	
	printf("\n");
	printf("Code downloaded to PXA and should be executing now\n");
	printf("Have a nice day\n");
	
	close(fd);
	closeport();
	return 0;
}

