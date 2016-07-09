#include <stdio.h>
#include <stdlib.h>

#include "libjtag.h"
#include "hamcop.h"


void
put_chain(unsigned char *chain, unsigned long pos, unsigned long value)
{
	int byte, bit;

	byte = pos/8;
	bit = pos%8;
	if(value)
		chain[byte] |= 1<<bit;
	else
		chain[byte] &= ~(1<<bit);
}

void
zero_chain(unsigned char *chain, unsigned long len)
{
	int i;
	for(i = 0 ; i < (len/8) + len%8?1:0 ; i++)
		chain[i] = 0;
}

void
one_chain(unsigned char *chain, unsigned long len)
{
	int i;
	for(i = 0 ; i < (len/8) + len%8?1:0 ; i++)
		chain[i] = 0xFF;
}

void
write_chain(unsigned char *chain, unsigned long len)
{
	int i;
	capture_dr();
	clockin(0,0);           // enter shift state
	for(i = 0 ; i < len ; i++){
		clockin(i == len-1, chain[i/8] >> (i%8) & 1);
	}
	clockin(1,0);           // enter update DR state
        clockin(0,0);           // enter run-test/idle state
	set_instr(EXTEST);
}

unsigned long
get_long_from_chain(unsigned char *chain, unsigned long pos)
{
	unsigned long out = 0;
	int i;

	for(i = pos ; i < pos+32 ; i++)
		out |= (chain[i/8] >> (i%8) & 1) << ((i-pos));
	return out;
}

void
read_chain(unsigned char *chain, unsigned long len)
{
        int i;
        capture_dr();
        for(i = 0 ; i < len ; i++) {
		clockin(i == len-1, 0);
		chain[i/8] &= ~(1 << (i%8));
		chain[i/8] |= clockout() << (i%8);
	}
       clockin(1,0);           // enter update DR state
       clockin(0,0);           // enter run-test/idle state
}

void
chain_through(unsigned char* ichain, unsigned char* ochain, unsigned long len)
{
	int i;
        capture_dr();
        for(i = 0 ; i < len ; i++) {
		clockin(i == len-1, ichain[i/8] >> (i%8) & 1);
		ochain[i/8] &= ~(1 << (i%8));
		ochain[i/8] |= clockout() << (i%8);
	}
	clockin(1,0);           // enter update DR state
	clockin(0,0);           // enter run-test/idle state
}

void
jtag_start(void)
{
	openport();
	test_reset();
	clockin(0,0);
}
