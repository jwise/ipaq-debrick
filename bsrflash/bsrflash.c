#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "bus.h"
#include "flash_device.h"

#define BLOCK_SIZE (128*1024)

extern struct bus *pxa_bus_probe(void);
extern struct flash_device *strataflash_probe(struct bus* bus);
extern struct flash_device *h1900_probe(struct bus* bus);

int main(int argc, char **argv) {
	struct flash_device *(*flash_probes[])(struct bus*) = {h1900_probe, strataflash_probe, NULL};
	struct flash_device *(**probe)(struct bus*);
	struct bus *bus;
	struct flash_device *dev;
	int fd;
	int i, size, blocks;
	unsigned long data, r_data;

	 if (argc < 2) {
                fprintf(stderr, "bsrflash: JTAG BSR flash tool\n"
                                "usage: bsrflash {p,v} <filename>      (program / verify)\n"
		                "       bsrflash r <filename> <size>   (read data)\n"
		                "       bsrflash u                     (unlock flash)\n");
                exit(1);
        }

	printf("Probing available busses... "); fflush(stdout);
	bus = pxa_bus_probe();
	if (!bus)
	{
		fprintf(stderr, "Bus probe failed.\n");
		exit(1);
	}
	bus->init();

	printf("Probing for devices... "); fflush(stdout);
	dev = NULL;
	probe = flash_probes;
	while(probe && !dev){
		dev = (*probe)(bus);
		probe++;
	}
	if (!dev)
	{
		fprintf(stderr, "No devices found.\n");
		exit(1);
	}

	if(argc < 3) { 
		if(argv[1][0] == 'u') {
			printf("Unlocking flash... "); fflush(stdout);
			dev->unlock(bus);  /* Unlock */
			printf("Done.\n");
		}
		else {
                        printf("Unknown operation\n");
                }
	}
	else if(argc < 4) { /* program / verify */
		if((fd=open(argv[2], O_RDONLY)) < 0){
			printf("Failed to open input file.\n");
			exit(1);
		}
		size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		if(size & 0x3){
                        printf("Size must be a multiple of 4\n");
                        exit(1);
                }
		blocks = (size/BLOCK_SIZE) + (size%BLOCK_SIZE?1:0);
	
		if(argv[1][0] == 'p') {  /* Program mode */
			printf("Beginning programming...\n");
			if(dev->start)
				dev->start(bus);

			for(i = 0 ; i < blocks ; i++) {
				printf("Erasing block: %d / %d\r", i+1, blocks); fflush(stdout);
				dev->erase_block(bus, i);
			}
			printf("\n");
			
			for(i = 0 ; i < size ; i+=4) {
				if(read(fd, &data, 4) < 4) {
					printf("\nShort read! Data may be corrupt!\n");
					break;
				}
				printf("Programming: %08x / %08x\r", i, size-4); fflush(0);
				dev->write_word(bus, i, data);
			}
			printf("\nDone!\n");
			
			if(dev->end)
				dev->end(bus);
		}
		else if(argv[1][0] == 'v') {  /* Verify mode */
			printf("Beginning verify...\n");
			for(i = 0 ; i < size ; i+=4) {
				fflush(stdout);
				if(read(fd, &data, 4) < 4) { 
					printf("\rShort read at %08x - verify aborted\n", i);
					break;
				}
				if(data != (r_data = dev->read_word(bus, i))) {
					printf("Verify: error at %08x! Read: %08x. Expect: %08x\n", i, (int)r_data, (int)data);
				}
				else {
					printf("Verifying: %08x / %08x         \r", i, size-4); fflush(0);
				}
			}
			printf("\nDone!\n");
		}
		else {
			printf("Unknown operation\n");
		}
		close(fd);
	}
	
	else if(argc < 5 && argv[1][0] == 'r') { /* read to file mode */
		size = strtoul(argv[3], 0, 0);
		if(size & 0x3){
			printf("Size must be a multiple of 4\n");
			exit(1);
		}
		if((fd = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) {
			printf("Failed to open output file.\n");
			exit(1);
		}
		printf("Reading data: Beginning read...\r");
		for(i = 0 ; i < size ; i+=4) {
			printf("Reading data:  %08x / %08x       \r", i, size-4); fflush(0);
			data = dev->read_word(bus, i);
			if(write(fd, &data, 4) < 4){
				printf("Reading data: Write error at %08x.\n", i);
				close(fd);
				exit(1);
			}
		}
		close(fd);
		printf("Reading data: done                          \n");
	}
	else {
		printf("Unknown operation\n");
	}

	return 0;
}

