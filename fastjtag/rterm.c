#include "libjtag.c"
#include <sys/termios.h>

int main(int argc, char** argv)
{
	unsigned long data;
	struct termios term;
	char data2;
	int hasdat;
	int fd;
	
	if (argc != 1)
	{
usage:
		fprintf(stderr,
			"%s: a program to watch for stuff coming out over the console over jtag.\n", argv[0]);
		exit(1);
	}
	
	openport();
	
	test_reset();		// returns us in reset state
	clockin(0, 0);		// this leaves us in run-test/idle
	look_for_pxa();		// returns us in run-test/idle state
	
	set_instr(DBGTX);	
	/* We do not do any of the high-level stuff here. Just wait for RR and read. */
	while(1)
	{
		int lcount = 0;
		hasdat = 0;
		while (!hasdat)
		{
			hasdat = gettxword(&data);
			if (!hasdat)
				lcount++;
			if (lcount > 10)
				set_instr(DBGTX);
		}
 		data2 = data & 0xFF;
 		write(1, &data2, 1);
	}
	
	closeport();
	return 0;
}
