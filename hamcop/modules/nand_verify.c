#include <stdio.h>
#include <stdlib.h>

#include "../nand.h"
#include "../libjtag.h"
#include "../jtag_core.h"
#include "../hamcop.h"

void
nand_verify(void)
{
	unsigned char manuf, dev, data[528];
	int i;

//	nand_sane();
	nand_command(NAND_CMD_RESET);
	while (!nand_ready()) ;

	nand_command(NAND_CMD_READID);
	nand_address(0x00);
	manuf = nand_read("manuf id");
	dev = nand_read("device id");
	printf("ID: %02Xh/%02Xh (Manufacturer: %s, Device name: %s)\n", manuf, dev, get_manufacturer_name(manuf) ? : "Unknown", get_device_name(dev) ? : "Unknown");
	
	nand_command(NAND_CMD_RESET);
	while (!nand_ready()) ;
	
	nand_command(NAND_CMD_READA);
	nand_address(0x00);
	nand_address(0x00);
	nand_address(0x00);
	for (i=0; i<528; i++)
	{
		data[i] = nand_read("byte");
		printf("\rReading byte: %d / 528", i+1);
		fflush(stdout);
	}
	printf("\n");
	printf("Data: \t");
	for (i=0; i<512; i++) {
		printf("%02X ", data[i]);
		if (i) if ((i + 1) % 16 == 0) {
#if 0
			printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				data[i-15], data[i-14], data[i-13], data[i-12], data[i-11],
				data[i-10], data[i-9], data[i-8], data[i-7], data[i-6], 
				data[i-5], data[i-4], data[i-3], data[i-2], data[i-1], data[i]);
#endif
			printf("\n\t");
		}
	}
	printf("\nOOB: \t");
	for (i=512; i<528; i++) printf("%02X ", data[i]);
	printf("\n");
}
