#define H1900_FLASH
#undef  H3900_FLASH

#include "libjtag.c"

#ifdef H1900_FLASH
#  define FLASHSIZE (512*1024)
#endif

#ifdef H3900_FLASH
#  define FLASHSIZE (512*1024)
#endif

void readtofile(char* file);
void verifyfile(char* file);
void programfile(char* file);

int main(int argc, char** argv)
{
	unsigned long target;
	unsigned long data;
	int fd;
	
	if (argc < 3)
	{
usage:
		fprintf(stderr,
			"%s: a program to reflash a 'bricked' device without boundary-scan.\n"
			"usage: %s R filename - reads current flash contents back from a device.\n"
			"usage: %s V filename - verifies current flash contents against a file.\n"
			"usage: %s P filename - programs a new image into flash. (use with caution!)\n", argv[0], argv[0], argv[0], argv[0]);
		exit(1);
	}
	
	if (argv[1][1] != '\0')
		goto usage;
	if (argv[1][0] != 'R' && argv[1][0] != 'r' &&
	    argv[1][0] != 'V' && argv[1][0] != 'v' &&
	    argv[1][0] != 'P' && argv[1][0] != 'p')
	    	goto usage;
	
	openport();
	
	test_reset();		// returns us in reset state
	clockin(0, 0);		// this leaves us in run-test/idle
	look_for_pxa();		// returns us in run-test/idle state
	
	if ((data = jcheck()))
	{
		printf("JTAG Check FAIL: Expected 0x0FFFFFFE, got 0x%08X!\n", data);
		if (data == 0x1FFFFFFF)
			printf("Did you forget to load the client binary?\n");
		closeport();
		exit(0);
	}
	
	//jwrite(0x0C000228, 0x0);
	
	if (argv[1][0] == 'R' || argv[1][0] == 'r')
		readtofile(argv[2]);
	if (argv[1][0] == 'V' || argv[1][0] == 'v')
		verifyfile(argv[2]);
	if (argv[1][0] == 'P' || argv[1][0] == 'p')
		programfile(argv[2]);
		
	//jwrite(0x0C000228, 0x7);
	
	closeport();
	return 0;
}

void readtofile(char* file)
{
	int fd;
	unsigned long target;
	unsigned long data;
	
	fd = open(file, O_WRONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		printf("Failed to open file!\n");
		return;
	}
	
	jstartfastread(0x0, FLASHSIZE);
	
	for (target = 0; target < FLASHSIZE; target += 4)
	{
		if ((target & 0x1FF) == 0x0)
		{
			printf("\rReading: %08X / %08X (%0.3f%%)", target, FLASHSIZE, (float)target/(float)FLASHSIZE*100.0f);
			fflush(stdout);
		}
		
		data = jfastread();
		if (write(fd, &data, 4) < 4)
		{
			printf("WARNING: failed to write to file!\n");
			return;
		}
	}
	
	printf("\rReading: Done reading %08X bytes!                     \n", FLASHSIZE);
	close(fd);
}

void verifyfile(char* file)
{
	int fd;
	unsigned long target;
	unsigned long data, data2;
	unsigned long length;
	
	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		printf("Failed to open file!\n");
		return;
	}
	
	length = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	length = (length > FLASHSIZE) ? FLASHSIZE : length;
	
#ifdef H3900_FLASH
	jwrite(0x0, 0x00FF00FF);
#endif

	jstartfastread(0x0, length/4);
	
	for (target = 0; target < length; target += 4)
	{
		if (read(fd, &data, 4) < 4)
		{
			printf("WARNING: Failed to read from file!\n");
			return;
		}
		
		if ((target & 0x1FF) == 0x0)
		{
			printf("\rVerify: %08X / %08X (%0.3f%%)", target, length, (float)target/(float)length*100.0f);
			fflush(stdout);
		}
		
		data2 = jfastread();
		if (data != data2)
		{
			printf("\nVerify failure!\n");
			printf("Expected: %08X\n", data);
			printf("Read:     %08X\n", data2);
			close(fd);
			target += 4;
			for (;target < length; target += 4)
				jfastread();
			return;
		}
	}
	
	printf("\rVerify: Done verifying %08X bytes!                     \n", length);
	close(fd);
}

void programfile(char* file)
{
	int fd;
	unsigned long target;
	unsigned long data;
	unsigned long length;
	int block_no = -1;
	
#ifdef H1900_FLASH
	/* Try to detect flash */
	printf("Program: Detecting H1910 flash...");
	fflush(stdout);
	jwriteh(0x0, 0x00F000F0);
	jwriteh(0x555<<1, 0x00AA00AA);
	jwriteh(0x2AA<<1, 0x00550055);
	jwriteh(0x555<<1, 0x00900090);
	if ((jreadh(0x0 << 1) != 0x0001) || (jreadh(0x1 << 1) != 0x22B9))
	{
		printf("failed to detect flash (magic numbers were %04X, %04X)\n", jreadh(0x0 << 1), jreadh(0x1 << 1));
		return;
	}
	jwriteh(0x0, 0x00F000F0);
#endif
#ifdef H3900_FLASH
	printf("Program: Enabling VPP");
	jwrite(0x40e00018, 0x10000);
	jwrite(0x40f00004, 0x30);
	fflush(stdout);
#endif

	fd = open(file, O_RDONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		printf("Failed to open file!\n");
		return;
	}
	
	length = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	if (length > FLASHSIZE)
	{
		printf("File is too big:\n");
		printf("File size:  %08X\n", length);
		printf("Flash size: %08X\n", FLASHSIZE);
		close(fd);
		return;
	}
	if (length % 4)
	{
		printf("File size is not a multiple of 4.\n");
		close(fd);
		return;
	}
	
	for (target = 0; target < length; target += 4)
	{
		int newbn = 0;
		
		if (read(fd, &data, 4) < 4)
		{
			printf("WARNING: failed to read from file!\n");
			return;
		}
		
#ifdef H1900_FLASH
		if (target < 0x70000)
			newbn = target / 0x10000;
		if (target >= 0x70000 && target < 0x78000)
			newbn = 7;
		if (target >= 0x78000 && target < 0x7A000)
			newbn = 8;
		if (target >= 0x7A000 && target < 0x7C000)
			newbn = 9;
		if (target >= 0x7C000)
			newbn = 10;
			
		if (block_no != newbn)
		{
			if (block_no != -1)
			{
				printf("\rProgram: Ending write cycle                            ");
				fflush(stdout);
				jfastwrite(0xEFFFFFFF, 0xFFFFFFFF);
				jwriteh(0x0, 0x00900090);
				jwriteh(0x0, 0x00000000);
				jwriteh(0x0, 0x00F000F0);
				printf("\rProgram: Write cycle complete                            ");
				fflush(stdout);
			}
			
			block_no = newbn;
			
			//jwriteh(0x0, 0x00F000F0);
			
			/* now start erasing the block */
			jwriteh(0x555 << 1, 0x00AA00AA);
			jwriteh(0x2AA << 1, 0x00550055);
			jwriteh(0x555 << 1, 0x00800080);
			jwriteh(0x555 << 1, 0x00AA00AA);
			jwriteh(0x2AA << 1, 0x00550055);
			jwriteh((target + 0x000), 0x00300030);
			
			printf("\rProgram: Erasing block %d@%08X                            ", block_no, target);
			fflush(stdout);
			
			{
				int tmo = 15000;
				int y;
				while (((y = jreadh(target)) & 0xFF) != 0xFF)
				{
					tmo--;
					if (tmo == 0)
					{
						printf("\rProgram: Erase at address %08X failed! (%08x)\n", target, y);
						return;
					}
				}
			}
			
			jwriteh(0x555 << 1, 0x00AA00AA);
			jwriteh(0x2AA << 1, 0x00550055);
			jwriteh(0x555 << 1, 0x00200020);
			
			jstarth1910write();
			
			printf("\rProgram: Block erase complete                            ");
		}

#endif
#ifdef H3900_FLASH
		newbn = target / 0x40000;
		if (block_no != newbn)
		{
			printf("\rProgram: Erasing block %d@%08X                            ", block_no, target);
			fflush(stdout);
			
			if (block_no != -1)
				jfastwrite(0xEFFFFFFF, 0x0);
			
			jwrite(target, 0x00FF00FF);
			jwrite(target, 0x00500050);
			jwrite(target, 0x00FF00FF);
			
			printf("\rProgram: Unlocking block %d@%08X                                            ");
			fflush(stdout);
			
			jwrite(0x0, 0x00FF00FF);
			jwrite(0x0, 0x00600060);
			jread(0x0);
			jwrite(0x0, 0x00D000D0);
			{
				int tmo = 15000;
				int y;
				while (((y = jread(0)) & 0x00B000B0) != 0x00800080)
				{
					tmo--;
					printf("\rProgram: Unlocking block %d@%08X R %08X T %d                          ", block_no, target, y, tmo);
					fflush(stdout);
					if (tmo == 0)
					{
						printf("\rProgram: Unlock at address %08X failed! (%08x)\n", target, y);
						return;
					}
				}
			}
			
			jwrite(0x0, 0x00FF00FF);
			jwrite(0x0, 0x00500050);
			jwrite(0x0, 0x00FF00FF);

			block_no = newbn;
			printf("\rProgram: Erasing block %d@%08X R                          ", block_no, target);
			fflush(stdout);
			jwrite(target, 0x00200020);
			printf("\rProgram: Erasing block %d@%08X A                          ", block_no, target);
			fflush(stdout);
			jwrite(target, 0x00D000D0);
			
			printf("\rProgram: Erasing block %d@%08X G                          ", block_no, target);
			fflush(stdout);
			
			{
				int tmo = 15000;
				int y;
				while (((y = jread(target)) & 0x00B000B0) != 0x00800080)
				{
					tmo--;
					printf("\rProgram: Erasing block %d@%08X R %08X T %d                          ", block_no, target, y, tmo);
					fflush(stdout);
					if (tmo == 0)
					{
						printf("\rProgram: Erase at address %08X failed! (%08x)\n", target, y);
						return;
					}
				}
			}
			
			jwrite(0x0, 0x00FF00FF);
			jwrite(0x0, 0x00500050);
			jwrite(0x0, 0x00FF00FF);
			printf("\rProgram: Block erase complete                            ");
			jstarth3900write();
		}
#endif

		if ((target & 0x1F) == 0x0)
		{
			printf("\rProgram: %08X / %08X (%0.3f%%)", target, length, (float)target/(float)length*100.0f);
			fflush(stdout);
		}
		
		jfastwrite(target, data);
	}
	
#ifdef H1900_FLASH
	jfastwrite(0xEFFFFFFF, 0xFFFFFFFF);
	jwriteh(0x0, 0x00900090);
	jwriteh(0x0, 0x00000000);
	jwriteh(0x0, 0x00F000F0);
#endif
#ifdef H3900_FLASH
	jfastwrite(0xEFFFFFFF, 0xFFFFFFFF);
#endif
	
	printf("\rProgram: Done programming %08X bytes!                                 \n", length);
	
	verifyfile(file);
}
