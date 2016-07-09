#include <stdlib.h>
#include <stdio.h>
#include "bus.h"
#include "flash_device.h"

static int h1900flash_write_word(struct bus *bus, unsigned long addr, unsigned long data) {
	BUS_WRITE(bus, 0x555 << 1, 0x00AA00AAL);
	BUS_WRITE(bus, 0x2AA << 1, 0x00550055L);
	BUS_WRITE(bus, 0x555 << 1, 0x00A000A0L);
	BUS_WRITE(bus, addr+2, data >> 16);
	
	BUS_WRITE(bus, 0x555 << 1, 0x00AA00AAL);
	BUS_WRITE(bus, 0x2AA << 1, 0x00550055L);
	BUS_WRITE(bus, 0x555 << 1, 0x00A000A0L);
	BUS_WRITE(bus, addr, data & 0xFFFF);

	return 0;
}

static int h1900flash_erase_block(struct bus *bus, unsigned long block) {
	int res;
	BUS_WRITE(bus, 0x555 << 1, 0x00AA00AAL);
	BUS_WRITE(bus, 0x2AA << 1, 0x00550055L);
	BUS_WRITE(bus, 0x555 << 1, 0x00800080L);
	BUS_WRITE(bus, 0x555 << 1, 0x00AA00AAL);
	BUS_WRITE(bus, 0x2AA << 1, 0x00550055L);
	BUS_WRITE(bus, block, 0x00300030L);
	do {
		res = BUS_READ(bus, block);
		fflush(stdout);
	} while((res & 0xFF) != 0xFF);

	return 0;
}

static void h1900flash_end(struct bus *bus) {
	BUS_WRITE(bus, 0, 0x00ff00ffL); // Set array to read mode
	bus->end();
}

static unsigned long h1900flash_read_word(struct bus *bus, unsigned long addr) {
	return BUS_READ(bus, addr) | BUS_READ(bus, addr + 2) << 16;
}

static struct flash_device flash_h1900 = {
	.write_word = h1900flash_write_word,
	.erase_block = h1900flash_erase_block,
	.read_word = h1900flash_read_word,
	.end = h1900flash_end
};

struct flash_device *h1900_probe(struct bus* bus) {
	BUS_WRITE(bus, 0x0, 0x00F000F0L);
	BUS_WRITE(bus, 0x555 << 1, 0x00AA00AAL);
	BUS_WRITE(bus, 0x2AA << 1, 0x00550055L);
	BUS_WRITE(bus, 0x555 << 1, 0x00900090L);
	if (BUS_READ(bus, 0x0 << 1) != 0x0001)
		return NULL;
	if (BUS_READ(bus, 0x1 << 1) != 0x22B9)
		return NULL;
	BUS_WRITE(bus, 0x0, 0x00F000F0);
	
	printf("Found h1900\n");
	return &flash_h1900;
}

