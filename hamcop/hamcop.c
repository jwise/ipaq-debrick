#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include "libjtag.h"
#include "jtag_core.h"
#include "hamcop.h"
#include "nand.h"
#include "includes.h"

unsigned char ichain[HAMCOP_CHAIN_LEN/8+1];
unsigned char ochain[(HAMCOP_CHAIN_LEN/8)+1];

struct device_table devids[] = {
    { 0x98, 0x75, "TOSHIBA", "TC58DVM82A1FT series NAND" },
    { 0xEC, 0x75, "SAMSUNG", "K9F5608U0A series NAND" },
    { 0x00, 0x00, NULL, NULL },
};

int chain_flags = 0x0;

char *
get_device_name(int device_code)
{
	int i;
	
	for (i = 0; devids[i].device_code; i++)
		if (devids[i].device_code == device_code) return devids[i].device_name;

	return NULL;
}

char *
get_manufacturer_name(int manufacturer_code)
{
	int i;
	
	for (i = 0; devids[i].manufacturer_code; i++)
		if (devids[i].manufacturer_code == manufacturer_code) return devids[i].manufacturer_name;
	return NULL;
}

void
hamcop_init(void) {
	jtag_start();
//	test_reset();
//	set_instr(IDCODE);
	look_for_pxa(); // returns to idle state

	set_instr(SAMPLE);
}

void usage(char* name)
{
	printf("Usage: %s MODE [options]\n", name);
	printf("Where mode is one of:\n");
	printf("  -f              frob LEDs\n");
	printf("  -h              set chain high\n");
	printf("  -l              set chain low\n");
	printf("  -s              search chain for desired peripheral (get out your DMM!)\n");
	printf("  -2              search chain (mode 2) for desired peripheral (get out your DMM!)\n");
	printf("  -p              print chain\n");
	printf("  -r              reset TAP\n");
	printf("  -i              interactive\n");
	printf("  -g              look for garbage bits\n");
	printf("  -n              verify accessibility of nand by reading the first page\n");
	printf("  -u              dump nand\n");
	printf("  -b              brick device (i.e., erase first block)\n");
	printf("  -d [filename]   debrick device\n");
	printf("  -w [filename]   write image to flash\n");
	printf("Additionally, when searching chain, you can also specify a number after the mode to skip disabling a bit.\n");
	printf("You can also specify a second number to tell which bit to start on. All previous bits will be zeros.\n");
	printf("In search mode 2, you MUST specify an argument to tell which bit to keep enabled.\n");
}

int main(int argc, char** argv)
{
	if ((argc < 2) || (argv[1][0] != '-') || (argv[1][2] != '\0'))
	{
		usage(argv[0]);
		return 1;
	}
	
	switch(argv[1][1])
	{
	case 'f':
		hamcop_init();
		frob_leds();
		break;
	case 'h':
		hamcop_init();
		chain_high();
		break;
	case 'l':
		hamcop_init();
		chain_low();
		break;
	case 's':
		hamcop_init();
		search_chain((argc > 2) ? atoi(argv[2]) : -1, (argc > 3) ? atoi(argv[3]) : HAMCOP_CHAIN_LEN);
		break;
	case '2':
		if (argc != 3)
		{
			usage(argv[0]);
			return 1;
		}
		hamcop_init();
		search_chain_mode2(atoi(argv[2]));
		break;
	case 'p':
		hamcop_init();
		print_chain();
		break;
	case 'r':
		hamcop_init();
		test_reset();
		break;
	case 'i':
		hamcop_init();
		interactive();
		break;
	case 'g':
		hamcop_init();
		garbage();
		break;
	case 'n':
		hamcop_init();
		nand_verify();
		break;
	case 'u':
		hamcop_init();
		if (argc < 3)
		{
			usage(argv[0]);
			return 1;
		}
		nand_dump(argv[2], (argc > 3) ? strtoul(argv[3], NULL, 0) : 0, (argc > 4) ? strtoul(argv[4], NULL, 0) : -1);
		break;
	case 'b':
		hamcop_init();
		brick();
		break;
	case 'd':
		hamcop_init();
		if (argc != 3)
		{
			usage(argv[0]);
			return 1;
		}
		debrick(argv[2]);
		break;
	case 'w':
		hamcop_init();
		if (argc != 3)
		{
			usage(argv[0]);
			return 1;
		}
		flash(argv[2]);
		break;
	default:
		usage(argv[0]);
		return 1;
	}

	closeport();

	return 0;
}
