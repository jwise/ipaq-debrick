#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "../nand.h"
#include "../jtag_core.h"
#include "../libjtag.h"
#include "../hamcop.h"

void
debrick(char* filename)
{
	unsigned char status;
	int i;
	unsigned char databuf[512] = {0};
	unsigned char oobbuf[16]   = {0};
	int ofs = 0;
	int oobblock = 512;
	int len;
	
	int fd;
	if ((fd = open(filename, O_RDONLY)) < 0)
	{
		perror("open");
		exit(2);
	}
	
	printf("Now debricking...\n");
	nand_command(NAND_CMD_RESET);
	while (!nand_ready())
		;
		
	ofs = 0;
	memset(databuf, 0xFF, 512);
	while ((len = read(fd, databuf, 512)) != 0)
	{
		if (len < 512)
			printf("Short read, but continuing anyway...\n");
		memset(oobbuf, 0xFF, 16);
		read(fd, oobbuf, 16);
		
		chain_flags = LED_GREEN;
//		nand_command(NAND_CMD_READA);
		nand_command(NAND_CMD_PROG1);
		nand_address(0x00);
		nand_address((ofs >> 9) & 0xFF);
		nand_address((ofs >> 17) & 0xFF);
		for (i=0; i < 512; i++)
		{
			printf("\r%3d: %3d/512: %02x", ofs / oobblock, i, databuf[i]);
			fflush(stdout);
			nand_write(databuf[i]);
		}
		chain_flags = LED_AMBER;
		printf("\r                      ");
		for (i=0; i < 16; i++)
		{
			printf("\r%3d: %3d/ 16: %02x", ofs / oobblock, i, oobbuf[i]);
			fflush(stdout);
			nand_write(oobbuf[i]);
		}
		printf("\r                      ");
		nand_command(NAND_CMD_PROG2);
		while (!nand_ready())
			;
		nand_command(NAND_CMD_STATUS);
		status = nand_read("status");
		if (status & 0x1)
		{
			chain_flags = LED_AMBER | LED_GREEN;
			nand_sane();
			printf("\rError while debricking! Offset: %08x\n", ofs);
			exit(1);
		}
		ofs += oobblock;
		memset(databuf, 0xFF, 512);
	};
	chain_flags = LED_BLUE;
	nand_sane();
	printf("\rSuccessfully debricked your iPAQ. Enjoy your new PDA.\n");
}
