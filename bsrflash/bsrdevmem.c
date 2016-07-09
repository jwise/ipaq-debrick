#include <stdio.h>
#include <stdlib.h>

#include "bus.h"
#include "flash_device.h"

extern struct bus *pxa_bus_probe(void);
extern struct flash_device *strataflash_probe(struct bus* bus);
extern struct flash_device *h1900_probe(struct bus* bus);

int main(int argc, char** argv) {
	struct bus *bus;
//	struct flash_device *dev;
	unsigned long addr, data;

	printf("Probing available busses... "); fflush(stdout);
	bus = pxa_bus_probe();
	if (!bus)
	{
		fprintf(stderr, "Bus probe failed.\n");
		exit(1);
	}
	bus->init();

	if (argc < 2) {
		fprintf(stderr, "bsrdevmem: read/write the memory bus via JTAG BSR\n"
				"usage: bsrdevmem <address> [<data>]\n");
		exit(1);
	}
	
	addr = strtoul(argv[1], 0, 0);

	if(addr & 0x3) {
		fprintf(stderr, "Address must be word aligned\n");
		exit(1);
	}


	if(argc < 3){ /* Read */
		printf("Read: addr=%08x  data=%08x\n", (int)addr, (int)BUS_READ(bus, addr));
	}
	else if(argc < 4) { /* Write */
		data = strtoul(argv[2], 0, 0);
		printf("Write: addr=%08x  data=%08x ... ", (int)addr, (int)data); fflush(stdout);
		BUS_WRITE(bus, addr, data);
		printf("Done\n");
	}
	
	return 0;
}

