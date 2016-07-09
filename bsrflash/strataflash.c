#include <stdio.h>

#include "bus.h"
#include "flash_device.h"

//#define DEBUG(a) printf(a)
#define DEBUG(a)

#define STRATA_J3 0
#define STRATA_K3 1

static int type;

static int sflash_write_word(struct bus *bus, unsigned long addr, unsigned long data) {
	int res;
	DEBUG("write_word\n");
	BUS_WRITE(bus, 0, 0x00500050L);    //clear status

	BUS_WRITE(bus, addr, 0x00400040L); // write word command
	BUS_WRITE(bus, addr, data);        // write data

	// Check status
        BUS_WRITE(bus, 0, 0x00700070L);
        do {
                res = BUS_READ(bus, 0);
        } while((res & 0x00800080) != 0x00800080);

	return 0;
}

static int sflash_erase_block(struct bus *bus, unsigned long block) {
	int res;
	DEBUG("erase_block\n");
	BUS_WRITE(bus, 0, 0x00500050L);     // clear status

	BUS_WRITE(bus, block, 0x00200020L); // erase block command
	BUS_WRITE(bus, block, 0x00D000D0L); // confirm erase
	
	// Check status
	BUS_WRITE(bus, 0, 0x00700070L);
	do {
		res = BUS_READ(bus, 0);
	} while((res & 0x00800080) != 0x00800080);

	return 0;
}

static void sflash_end(struct bus *bus) {
	DEBUG("end\n");
	BUS_WRITE(bus, 0, 0x00ff00ffL); // Set array to read mode
	bus->end();
}

static int sflash_unlock(struct bus *bus) {   /* Strataflash can only unlock all blocks */
        int res;
	DEBUG("unlock\n");
        BUS_WRITE(bus, 0, 0x00500050L);     // clear status
                                                                                
        BUS_WRITE(bus, 0, 0x00600060L); // erase block command
        BUS_WRITE(bus, 0, 0x00D000D0L); // confirm erase
                                                                                
        // Check status
        BUS_WRITE(bus, 0, 0x00700070L);
        do {
                res = BUS_READ(bus, 0);
        } while((res & 0x00800080) != 0x00800080);
                                                                                
        return 0;
}

static unsigned long sflash_read_word(struct bus *bus, unsigned long addr)
{
	DEBUG("read_word\n");
	return BUS_READ(bus, addr);
}

static struct flash_device flash_strata = {
	.write_word  = sflash_write_word,
	.erase_block = sflash_erase_block,
	.read_word   = sflash_read_word,
	.end         = sflash_end,
	.unlock      = sflash_unlock
};

struct flash_device *strataflash_probe(struct bus *bus) {
	int id;

	BUS_WRITE(bus, 0, 0x00900090);

	id = BUS_READ(bus, 0);

	if((id & 0xff) != 0x89)
		return NULL;
	
	if((id & 0x00ff00ff) == 0x00890089){
		printf("Found Intel ");
		id = BUS_READ(bus, 4);
		switch((id & 0xff00) >> 8){
			case 0x88:
				printf("K3");
				type = STRATA_K3;
			break;
			case 0x00:
				printf("J3");
				type = STRATA_J3;
			break;
		}
		printf(" strataflash in 2x16 bit mode.\n");
	}
	else{
		printf("Single-chip configuration not supported.\n");
		return NULL;
	}
	
	BUS_WRITE(bus, 0, 0x00ff00ffL); // Set array to read mode
	return &flash_strata;
}

