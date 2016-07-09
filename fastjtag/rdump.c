#include "libjtag.c"

void readtofile(char* file, unsigned long addr1, unsigned long addr2);

int main(int argc, char** argv)
{
	unsigned long ad1;
	unsigned long ad2;
	unsigned long data;
	int fd;
	
	if (argc != 4)
	{
usage:
		fprintf(stderr,
			"%s: a program to dump an area of memory over JTAG.\n"
			"usage: %s file startaddr sz\n", argv[0], argv[0]);
		exit(1);
	}
	
	ad1 = strtoul(argv[2], 0, 0);
	ad2 = strtoul(argv[3], 0, 0);
	
	openport();
	
	test_reset();		// returns us in reset state
	clockin(0, 0);		// this leaves us in run-test/idle
	look_for_pxa();		// returns us in run-test/idle state

	printf("Checking for memclient.bin... "); fflush(0);
	if ((data = jcheck()))
	{
		printf("JTAG Check FAIL: Expected 0x0FFFFFFE, got 0x%08X!\n", data);
		if (data == 0x1FFFFFFF)
			printf("Did you forget to load the client binary?\n");
		closeport();
		exit(0);
	}
	printf("Done.\n");

	readtofile(argv[1], ad1, ad2+ad1);
	
	closeport();
	return 0;
}

void readtofile(char* file, unsigned long addr1, unsigned long addr2)
{
	int fd;
	unsigned long target;
	unsigned long data;
	
	fd = open(file, O_WRONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		printf("Failed to open file!\n");
		return;
	}
	
	jstartfastread(addr1, (addr2-addr1)>>2);

	printf("Starting read from %08x to %08x\n", addr1, addr2);

	for (target = addr1; target < addr2; target += 4)
	{
		if ((target & 0x1FF) == 0x0)
		{
			printf("\rRead: %08X (%0.3f%%)", target-addr1, (float)(target-addr1)/(float)(addr2-addr1)*100.0f);
			fflush(stdout);
		}
		
		data = jfastread();
		if (write(fd, &data, 4) < 4)
		{
			printf("WARNING: failed to write to file!\n");
			return;
		}
	}
	
	printf("\rReading: Done reading %08X bytes!                     \n", addr2-addr1);
	close(fd);
}
