#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../nand.h"
#include "../jtag_core.h"
#include "../libjtag.h"
#include "../hamcop.h"

void
brick(void)
{
	unsigned char status;
	
	printf("Now bricking...\n");
	chain_flags = LED_GREEN;
	nand_command(NAND_CMD_RESET);
	while (!nand_ready())
		;
	nand_command(NAND_CMD_ERASE1);
	nand_address(0x00);
	nand_address(0x00);
	nand_command(NAND_CMD_ERASE2);
	while (!nand_ready())
		;
	nand_command(NAND_CMD_STATUS);
	status = nand_read("status");
	if (status & 0x1)
	{
		chain_flags = LED_AMBER;
		printf("Error while bricking!\n");
	}
	else
	{
		chain_flags = LED_BLUE;
		printf("Successfully bricked your iPAQ. Enjoy your new doorstop.\n");
	}
	nand_sane();
}
