#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../nand.h"
#include "../jtag_core.h"
#include "../libjtag.h"
#include "../hamcop.h"

void
frob_leds(void)
{
	set_instr(PRELOAD);

	printf("Asserting LEDs in order...\n");
	zero_chain(ochain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);

	printf("[enable]\n");
	put_chain(ochain, HAMCOP_LED_ENABLE, 1);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	sleep(2);

	printf("[amber]\n");
	put_chain(ochain, HAMCOP_LED_AMBER, 1);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	sleep(2);
	put_chain(ochain, HAMCOP_LED_AMBER, 0);

	printf("[green]\n");
	put_chain(ochain, HAMCOP_LED_GREEN, 1);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	sleep(2);
	put_chain(ochain, HAMCOP_LED_GREEN, 0);

	printf("[blue]\n");
	put_chain(ochain, HAMCOP_LED_BLUE, 1);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	sleep(2);
	put_chain(ochain, HAMCOP_LED_BLUE, 0);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
}
