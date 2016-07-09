/* WIGGLER C file */
#include "gtag.h"
#include "wiggler.h"

#define WIGGLER_PROG 7
#define WIGGLER_TCK  2
#define WIGGLER_TMS  1
#define WIGGLER_TDI  3
#define WIGGLER_TDO  7

void cable_wiggler_clockin(int tms, int tdi)
{
	tms = tms ? 1 : 0;
	tdi = tdi ? 1 : 0;
	
	parallel_writedata((1 << WIGGLER_PROG) | (0 << WIGGLER_TCK) | (tms << WIGGLER_TMS) | (tdi << WIGGLER_TDI));
	if (globdbg) printf("0: tms %d tdi %d tdo X\n", tms, tdi);
	parallel_writedata((1 << WIGGLER_PROG) | (1 << WIGGLER_TCK) | (tms << WIGGLER_TMS) | (tdi << WIGGLER_TDI));
	if (globdbg) printf("1: tms %d tdi %d tdo X\n", tms, tdi);
}

unsigned char cable_wiggler_clockout()
{
	unsigned char data;
	
	parallel_writedata((1 << WIGGLER_PROG) | (0 << WIGGLER_TCK));
	data = parallel_readstatus();
	data ^= 0x80;
	data >>= WIGGLER_TDO;
	data &= 1;
	if (globdbg) printf("0: tms X tdi X tdo %d\n", data);

	return data;
}
