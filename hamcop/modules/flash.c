/* 2004, Szabolcs Gyurko */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include "../nand.h"
#include "../libjtag.h"
#include "../jtag_core.h"
#include "../hamcop.h"

void
flash(char* filename)
{
	unsigned char status, databuf[NAND_PAGE_SIZE] = {0}, oobbuf[NAND_OOB_SIZE] = {0};
	int i, ofs = 0, len, fd, manuf, dev;
	
	if ((fd = open(filename, O_RDONLY)) < 0) {
		perror("open");
		exit(2);
	}

//	nand_sane();
	nand_command(NAND_CMD_RESET);
	while (!nand_ready()) ;

	nand_command(NAND_CMD_READID);
	nand_address(0x00);
	manuf = nand_read("manuf id");
	dev = nand_read("device id");
	
	printf("Flashing your image (%s) to %s %s\n", filename, get_manufacturer_name(manuf), get_device_name(dev));

	ofs = 0;
	memset(databuf, 0xFF, NAND_PAGE_SIZE);
	while ((len = read(fd, databuf, NAND_PAGE_SIZE)) != 0) {
		memset(oobbuf, 0xFF, NAND_OOB_SIZE);
		read(fd, oobbuf, NAND_OOB_SIZE);

		/* Let's erase the block if we're in a new block */
		if ((ofs / NAND_PAGE_SIZE) % NAND_BLOCK_SIZE == 0) {
			int block;

			block = (ofs / NAND_PAGE_SIZE) / NAND_BLOCK_SIZE;
			printf("Erasing block %d... ", block);
			fflush(stdout);
			chain_flags = LED_BLUE;
			
			nand_command(NAND_CMD_ERASE1);
			nand_address(block & 0xFF);
			nand_address((block >> 8) & 0xFF);
			nand_command(NAND_CMD_ERASE2);
			usleep(NAND_ERASE_TIME);
			
			nand_command(NAND_CMD_STATUS);
			status = nand_read("status");
			if (status & 0x1) {
				chain_flags = LED_AMBER;
				printf("Error while erasing block %d!\n", block);
			}
			
			printf("done.\n");
		}

		chain_flags = LED_GREEN;
		nand_command(NAND_CMD_PROG1);
		nand_address(ofs & 0xFF);
		nand_address((ofs >> 9) & 0xFF);
		nand_address((ofs >> 17) & 0xFF);

		for (i=0; i < NAND_PAGE_SIZE; i++) {
			nand_write(databuf[i]);
			printf("\r%3d: %3d/%d: %02x", ofs / NAND_PAGE_SIZE, i, NAND_PAGE_SIZE, databuf[i]);
			fflush(stdout);
		}

		chain_flags = LED_AMBER;
		printf("\r                      ");
		for (i=0; i < NAND_OOB_SIZE; i++) {
			nand_write(oobbuf[i]);
			printf("\r%3d: %3d/ %d: %02x", ofs / NAND_PAGE_SIZE, i, NAND_OOB_SIZE, oobbuf[i]);
			fflush(stdout);
		}
		printf("\r                      ");
		nand_command(NAND_CMD_PROG2);
		
		usleep(NAND_PROG_TIME);	/* Defined in nand.h */

		nand_command(NAND_CMD_STATUS);
		status = nand_read("status");
		if (status & 0x1) {
			chain_flags = LED_AMBER | LED_GREEN;
			nand_sane();
			printf("\rError while flashing! Offset: %08x\n", ofs);
			exit(1);
		}
		ofs += NAND_PAGE_SIZE;
		memset(databuf, 0xFF, NAND_PAGE_SIZE);
	};
	chain_flags = LED_BLUE;
	nand_sane();
	printf("\rYour image is successfully written to your iPAQ. Enjoy your new PDA.\n");
}
