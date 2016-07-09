#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

#include "../nand.h"
#include "../libjtag.h"
#include "../jtag_core.h"
#include "../hamcop.h"

void
nand_dump(char* dumpname, int saddr, int length)
{
	int oobsize  = 16;
	int oobblock = 512;
	int flashsz  = 32*1024*1024;
	unsigned long ofd;
	int start_addr, end_addr;
	int pretty_print;
	int ofs, i;
	unsigned char readbuf[512];
	unsigned char oobbuf[16];
	unsigned char pretty_buf[80];
	
	if (strcmp(dumpname, "-") == 0)
		ofd = 1;
	else if ((ofd = open(dumpname, O_WRONLY | O_TRUNC | O_CREAT, 0644)) == -1)
	{
		perror("open outfile");
		exit(2);
	}
		
	start_addr = saddr & ~(oobblock - 1);
	end_addr   = (length == -1) ? flashsz : ((start_addr + length) & ~(oobblock - 1));
	
	fprintf(stderr, "Would you like formatted output? ");
	pretty_print = tolower(getc(stdin)) == 'y';
	fprintf(stderr, "Dumping data starting at 0x%08x and ending at 0x%08x...\n", start_addr, end_addr);

	// use auto-incr	
	nand_command(NAND_CMD_READA);
	nand_address(0x00);
	nand_address((start_addr >> 9) & 0xFF);
	nand_address((start_addr >> 17) & 0xFF);
	
	for (ofs = start_addr; ofs < end_addr; ofs += oobblock) {
		while (!nand_ready())
			;
		
		chain_flags = LED_GREEN;
		for (i=0; i<oobblock; i++)
		{
			fprintf(stderr, "Reading: 0x%08x + %d                        \r", ofs, i);
			fflush(stderr);
			readbuf[i] = nand_read("byte");
		}
		if (pretty_print)
		{
			for (i=0; i<oobblock; i+=16)
			{
				sprintf(pretty_buf,
					"0x%08x: %02x %02x %02x %02x %02x %02x %02x "
					"%02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
					(unsigned int) (ofs + i),  readbuf[i],
					readbuf[i+1], readbuf[i+2],
					readbuf[i+3], readbuf[i+4],
					readbuf[i+5], readbuf[i+6],
					readbuf[i+7], readbuf[i+8],
					readbuf[i+9], readbuf[i+10],
					readbuf[i+11], readbuf[i+12],
					readbuf[i+13], readbuf[i+14],
					readbuf[i+15]);
				write(ofd, pretty_buf, 60);
			}
		} else
			write(ofd, readbuf, oobblock);
		
		chain_flags = LED_AMBER;
		for (i=0; i<oobsize; i++)
		{
			fprintf(stderr, "Reading: 0x%08x + (%d)                        \r", ofs, i);
			fflush(stderr);
			oobbuf[i] = nand_read("oob byte");
		}
		if (pretty_print)
		{
			if (oobsize == 16) {
				sprintf(pretty_buf, "  OOB Data: %02x %02x %02x %02x %02x %02x "
					"%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
					oobbuf[0], oobbuf[1], oobbuf[2],
					oobbuf[3], oobbuf[4], oobbuf[5],
					oobbuf[6], oobbuf[7], oobbuf[8],
					oobbuf[9], oobbuf[10], oobbuf[11],
					oobbuf[12], oobbuf[13], oobbuf[14],
					oobbuf[15]);
				write(ofd, pretty_buf, 60);
			} else {
				sprintf(pretty_buf, "  OOB Data: %02x %02x %02x %02x %02x %02x "
					"%02x %02x\n",
					oobbuf[0], oobbuf[1], oobbuf[2],
					oobbuf[3], oobbuf[4], oobbuf[5],
					oobbuf[6], oobbuf[7]);
				write(ofd, pretty_buf, 48);
			}
		} else
			write(ofd, oobbuf, oobsize);
	}
	chain_flags = LED_BLUE;
	nand_sane();
	fprintf(stderr, "Reading: done!                             \n");
	close(ofd);
	return;
}
