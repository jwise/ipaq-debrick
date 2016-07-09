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
print_chain()
{
	int i;
	
	read_chain(ochain, HAMCOP_CHAIN_LEN);
	for (i = 0; i < HAMCOP_CHAIN_LEN; i++)
		printf("%d", (ochain[i/8] >> (i%8)) & 0x1);
	printf("\n");
}

void
chain_high()
{
	printf("Setting chain all high...\n");
	one_chain(ochain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
}

void
chain_low()
{
	printf("Setting chain all low...\n");
	zero_chain(ochain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
}

void
search_chain(int skipbit, int startbit)
{
	int i;
	char foo[16];	// insecure, but who cares? if this is suid root on your system and accessible to users, you're insane
	
	printf("Hit enter when ready to start deasserting bits, starting from %d, working our way down to %d...", startbit, 0);
	fflush(stdout);
	fgets(foo, sizeof(foo), stdin);
	one_chain(ochain, HAMCOP_CHAIN_LEN);
	for (i=HAMCOP_CHAIN_LEN; i > startbit; i--)
		put_chain(ochain, i, 0);
	for (i=startbit; i > 0; i--)
	{
		if (i == skipbit)
		{
			printf("Skipping disable for bit %d\n", i);
			continue;
		}
		put_chain(ochain, i, 0);
		write_chain(ochain, HAMCOP_CHAIN_LEN);
		printf("Deasserted %d", i);
		fflush(stdout);
		fgets(foo, sizeof(foo), stdin);
	}
}

void
search_chain_mode2(int constbit)
{
	int i;
	char foo[16];	// insecure, but who cares? if this is suid root on your system and accessible to users, you're insane
	
	printf("Hit enter when ready to start flipping through bits, starting from %d, working our way up to %d...", 0, HAMCOP_CHAIN_LEN);
	fflush(stdout);
	fgets(foo, sizeof(foo), stdin);
	for (i=0; i < HAMCOP_CHAIN_LEN; i++)
	{
		zero_chain(ochain, HAMCOP_CHAIN_LEN);
		put_chain(ochain, i, 1);
		// guard against up-screwage
		put_chain(ochain, 100, 1);	// nCE
		put_chain(ochain, 99, 1);	// nRE
		put_chain(ochain, 98, 1);	// nWE
		put_chain(ochain, constbit, 1);
		write_chain(ochain, HAMCOP_CHAIN_LEN);
		printf("Flipped %d", i);
		fflush(stdout);
		fgets(foo, sizeof(foo), stdin);
	}
}
