#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jtag_core.h"
#include "hamcop.h"
#include "nand.h"

#ifdef DEBUG
#  define DEBUG(x, args...) fprintf(stderr, x, args...)
#else
#  define DEBUG(x, args...)
#endif

void
setup_chain_with_flags(unsigned char* chain, int length)
{
	zero_chain(chain, length);
	put_chain(chain, HAMCOP_LED_ENABLE, 1);
	put_chain(chain, HAMCOP_LED_BLUE, chain_flags & LED_BLUE);
	put_chain(chain, HAMCOP_LED_AMBER, chain_flags & LED_AMBER);
	put_chain(chain, HAMCOP_LED_GREEN, chain_flags & LED_GREEN);
}

void
nand_data_to_chain(unsigned char* chain, unsigned char data)
{
	put_chain(chain, HAMCOP_NAND_IO_0_OUT, (data >> 0) & 0x1);
	put_chain(chain, HAMCOP_NAND_IO_1_OUT, (data >> 1) & 0x1);
	put_chain(chain, HAMCOP_NAND_IO_2_OUT, (data >> 2) & 0x1);
	put_chain(chain, HAMCOP_NAND_IO_3_OUT, (data >> 3) & 0x1);
	put_chain(chain, HAMCOP_NAND_IO_4_OUT, (data >> 4) & 0x1);
	put_chain(chain, HAMCOP_NAND_IO_5_OUT, (data >> 5) & 0x1);
	put_chain(chain, HAMCOP_NAND_IO_6_OUT, (data >> 6) & 0x1);
	put_chain(chain, HAMCOP_NAND_IO_7_OUT, (data >> 7) & 0x1);
}

unsigned char
nand_data_from_chain(unsigned char* chain)
{
	unsigned char d = 0;
	
	d |= ((chain[HAMCOP_NAND_IO_0_IN/8] >> (HAMCOP_NAND_IO_0_IN%8)) & 0x1) << 0;
	d |= ((chain[HAMCOP_NAND_IO_1_IN/8] >> (HAMCOP_NAND_IO_1_IN%8)) & 0x1) << 1;
	d |= ((chain[HAMCOP_NAND_IO_2_IN/8] >> (HAMCOP_NAND_IO_2_IN%8)) & 0x1) << 2;
	d |= ((chain[HAMCOP_NAND_IO_3_IN/8] >> (HAMCOP_NAND_IO_3_IN%8)) & 0x1) << 3;
	d |= ((chain[HAMCOP_NAND_IO_4_IN/8] >> (HAMCOP_NAND_IO_4_IN%8)) & 0x1) << 4;
	d |= ((chain[HAMCOP_NAND_IO_5_IN/8] >> (HAMCOP_NAND_IO_5_IN%8)) & 0x1) << 5;
	d |= ((chain[HAMCOP_NAND_IO_6_IN/8] >> (HAMCOP_NAND_IO_6_IN%8)) & 0x1) << 6;
	d |= ((chain[HAMCOP_NAND_IO_7_IN/8] >> (HAMCOP_NAND_IO_7_IN%8)) & 0x1) << 7;
	return d;
}

void nand_sane()
{
	DEBUG("[putting chip into sane state]\n");
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nCE, 1);
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 0);
	nand_data_to_chain(ochain, 0x00);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
}

void nand_command(unsigned char cmd)
{
	DEBUG("[writing command %02Xh]\n", cmd);
#if NO_TIMING
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 1);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_nWE, 0);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 0);
	nand_data_to_chain(ochain, cmd);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
#else	
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 1);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	NANOSLEEP(NAND_ALE_SETUP_TIME);
	
	put_chain(ochain, HAMCOP_NAND_nWE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 0);
	nand_data_to_chain(ochain, cmd);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);

	NANOSLEEP(NAND_WRITE_PULSE_TIME);
	
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
		
	NANOSLEEP(NAND_WE_HOLD_TIME);
#endif
}

void nand_address(unsigned char addr)
{
	DEBUG("[writing address %02Xh]\n", addr);

#if NO_TIMING
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_nWE, 0);
	put_chain(ochain, HAMCOP_NAND_ALE, 1);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 0);
	nand_data_to_chain(ochain, addr);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
#else	
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_ALE, 1);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	
	NANOSLEEP(NAND_ALE_SETUP_TIME);
	
	put_chain(ochain, HAMCOP_NAND_nWE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 0);
	nand_data_to_chain(ochain, addr);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	
	NANOSLEEP(NAND_WRITE_PULSE_TIME);
	
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);

	NANOSLEEP(NAND_WE_HOLD_TIME);
#endif
}

unsigned char nand_read(char* descr)
{
	unsigned char d;
	DEBUG("[reading %s]\n", descr);
	
#if NO_TIMING
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 0);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 1);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	read_chain(ichain, HAMCOP_CHAIN_LEN);
	d = nand_data_from_chain(ichain);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	write_chain(ichain, HAMCOP_CHAIN_LEN);
#else
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 0);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 1);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	d = nand_data_from_chain(ichain);
	
	NANOSLEEP(NAND_READ_PULSE_TIME);
	
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	
	NANOSLEEP(NAND_WE_HOLD_TIME);
#endif
	
	return d;
}

void nand_write(unsigned char d)
{
	DEBUG("[writing %02Xh]\n", d);

#if NO_TIMING
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_nWE, 0);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 0);
	nand_data_to_chain(ochain, d);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
//	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	write_chain(ochain, HAMCOP_CHAIN_LEN);
#else
	
	setup_chain_with_flags(ochain, HAMCOP_CHAIN_LEN);
	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	
	NANOSLEEP(NAND_ALE_SETUP_TIME);

	put_chain(ochain, HAMCOP_NAND_CLE, 0);
	put_chain(ochain, HAMCOP_NAND_nCE, 0);
	put_chain(ochain, HAMCOP_NAND_ALE, 0);
	put_chain(ochain, HAMCOP_NAND_nWE, 0);
	put_chain(ochain, HAMCOP_NAND_nRE, 1);
	put_chain(ochain, HAMCOP_NAND_nWP, 1);
	put_chain(ochain, HAMCOP_NAND_IO_HIGHZ, 0);
	nand_data_to_chain(ochain, d);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	
	NANOSLEEP(NAND_WRITE_PULSE_TIME);
	
	put_chain(ochain, HAMCOP_NAND_nWE, 1);
	chain_through(ochain, ichain, HAMCOP_CHAIN_LEN);
	
	NANOSLEEP(NAND_WE_HOLD_TIME);
#endif
}

int nand_ready()
{
	DEBUG("[checking whether device is ready]\n");
	read_chain(ichain, HAMCOP_CHAIN_LEN);
	return ((ichain[HAMCOP_NAND_READY/8] >> (HAMCOP_NAND_READY%8)) & 0x1) << 0;
}
