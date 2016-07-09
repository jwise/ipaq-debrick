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
interactive(void)
{
	char foo[16];	// insecure, but who cares? if this is suid root on your system and accessible to users, you're insane
	int bit;
	int val;
	
	zero_chain(ochain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	printf("Interactive mode\n");
	while(1)
	{
		printf("bit> ");
		fflush(stdout);
		fgets(foo, sizeof(foo), stdin);
		if (!foo[0])
		{
			printf("Ok, done. Bye.\n");
			return;
		}
		bit = atoi(foo);
		
		printf("val> ");
		fflush(stdout);
		fgets(foo, sizeof(foo), stdin);
		if (!foo[0])
		{
			printf("Ok, done. Bye.\n");
			return;
		}
		val = atoi(foo);
		put_chain(ochain, bit, val);
		write_chain(ochain, HAMCOP_CHAIN_LEN);
	}
		
}
