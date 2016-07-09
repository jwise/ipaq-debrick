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
garbage(void)
{
	char foo[16];	// insecure, but who cares? if this is suid root on your system and accessible to users, you're insane
	int bit,val;
	int i,j;
	unsigned char newochain[HAMCOP_CHAIN_LEN/8+1];
	unsigned char oldochain[HAMCOP_CHAIN_LEN/8+1];
	unsigned char garbage[HAMCOP_CHAIN_LEN];
	
	printf("Looking for garbage over the course of 500 loops...\n");
	set_instr(EXTEST);
	one_chain(ochain, HAMCOP_CHAIN_LEN);
	chain_through(ochain, newochain, HAMCOP_CHAIN_LEN);
	memcpy(oldochain, newochain, sizeof(newochain));
	memset(garbage, 0, sizeof(garbage));
	for (i=0; i<500; i++)
	{
		chain_through(ochain, newochain, HAMCOP_CHAIN_LEN);
		for (j = 0; j < HAMCOP_CHAIN_LEN; j++)
		{
			if (((newochain[j/8] >> (j%8)) & 0x1) != ((oldochain[j/8] >> (j%8)) & 0x1))
			{
				if (!garbage[j])
				{
					printf("Bit %d is garbage\n", j);
					garbage[j] = 1;
				}
			}
		}
		memcpy(oldochain, newochain, sizeof(newochain));
	}
	printf("Done\n");
	printf("Garbage bits: ");
	for(i=0;i<HAMCOP_CHAIN_LEN;i++)
	{
		if (garbage[i])
			printf("%d ", i);
	}
	printf("\n");
	zero_chain(ochain, HAMCOP_CHAIN_LEN);
	printf("Entering interactive mode\n");
	while(1)
	{
		set_instr(SAMPLE);
		chain_through(ochain, newochain, HAMCOP_CHAIN_LEN);
		for (j = 0; j < HAMCOP_CHAIN_LEN; j++)
		{
			if (((newochain[j/8] >> (j%8)) & 0x1) != ((oldochain[j/8] >> (j%8)) & 0x1))
				if (!garbage[j])
					printf("Bit %d changed - now %d\n", j, (newochain[j/8] >> (j%8)) & 0x1);
		}
		memcpy(oldochain, newochain, sizeof(newochain));
		
		printf("bit> ");
		fflush(stdout);
		fgets(foo, sizeof(foo), stdin);
		if (!foo[0])
			continue;
		if (foo[0] == 'q')
		{
			printf("Ok, done. Bye.\n");
			return;
		}
		bit = atoi(foo);
		
		printf("val> ");
		fflush(stdout);
		fgets(foo, sizeof(foo), stdin);
		if (foo[0] == 'q')
		{
			printf("Ok, done. Bye.\n");
			return;
		}
		val = atoi(foo);
		put_chain(ochain, bit, val);
	}
	
}
