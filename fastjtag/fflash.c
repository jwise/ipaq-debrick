/* fflash.c
 * (c) Ian Molton <spyro@f2s.com>
 * 
 * This can be used to program flash chips directly connected to a PXA CPU
 * Provided an appropriate stub (like memclient.S) is running on the target CPU.
 * Data is sent through the JTAG debug register, and this is *significantly*
 * Quicker than using a boundary scan method.
 *
 */

#include "libjtag.c"

#define BLOCK_SIZE (256*1024)

void write_flash(char* file, unsigned long offset);

int main(int argc, char** argv)
{
	unsigned long addr;
	unsigned long data;
	int fd;
	
	if (argc != 3)
	{
usage:
		fprintf(stderr,
			"fflash: Write a devices flash over jtag using an ARM\n"
			"        Stub to do the work on the PDA.\n"
			"usage: fflash <file> <dest address>\n");
		exit(1);
	}
	
	addr = strtoul(argv[2], NULL, 0);
	
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

	write_flash(argv[1], addr);
	
	closeport();
	return 0;
}

void erase_flash(int block) {
	set_instr(DBGRX);
        sendtxword(0x32);
        sendtxword(block);
}

void start_write(void) {
	set_instr(DBGRX);
        sendtxword(0x33);
}

void write_word(unsigned long addr, unsigned long data) {
	sendtxword(addr);
	sendtxword(data);
}

#define end_write() write_word(0x0ffffffe, 0)

void write_flash(char* file, unsigned long offset) {
	// 0x480000 - kernel
	// 0x40000  - params
	// 0x80000  - initrd
	// 0x0      - wince
	int fd, i, length, blocks, base_block;
	unsigned long addr;
	unsigned long data;
	
	if((fd = open(file, O_RDONLY)) < 0) {
		printf("Failed to open input file!\n");
		return;
	}
	
	length = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

	base_block = offset/BLOCK_SIZE;	
	blocks = (length/BLOCK_SIZE) + (length%BLOCK_SIZE?1:0);
	printf("Erasing blocks %d-%d...\n", base_block, base_block+blocks-1); fflush(stdout);
	for(i = 0 ; i < blocks ; i++){
		printf("\rBlock: %d (%d / %d)...", base_block+i, i+1, blocks); fflush(stdout);
		erase_flash((i+base_block)*BLOCK_SIZE);
	}
	printf("Done.\n");

	printf("Starting write... \n");
	lseek(fd, 0, SEEK_SET);

	start_write();

	for (addr = 0 ; addr < length; addr += 4)
	{
		if ((addr & 0xFF) == 0x0)
			printf("\rWrite: %08X (%0.3f%%)", offset+addr, (float)(addr*100)/length); fflush(stdout);
		
		if (read(fd, &data, 4) < 4)
		{
			printf("WARNING: short read data may be corrupt\n");
			return;
		}
		if(data != 0xffffffff)  // Dont write all 1s (flash default after erase)
			write_word(offset+addr, data);	
	}
	
	end_write();
	
	printf("\rWrite: Done writing %08X bytes!                     \n", length);
	close(fd);
}
