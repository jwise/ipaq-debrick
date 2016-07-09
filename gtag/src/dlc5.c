/* DLC5 C file */
#include "gtag.h"
#include "dlc5.h"

#define DLC5_PROG 4
#define DLC5_TCK  1
#define DLC5_TMS  2
#define DLC5_TDI  0
#define DLC5_TDO  4

void cable_dlc5_clockin(int tms, int tdi)
{
	tms = tms ? 1 : 0;
	tdi = tdi ? 1 : 0;
	
	parallel_writedata((1 << DLC5_PROG) | (0 << DLC5_TCK) | (tms << DLC5_TMS) | (tdi << DLC5_TDI));
	if (globdbg) printf("0: tms %d tdi %d tdo X\n", tms, tdi);
	parallel_writedata((1 << DLC5_PROG) | (1 << DLC5_TCK) | (tms << DLC5_TMS) | (tdi << DLC5_TDI));
	if (globdbg) printf("1: tms %d tdi %d tdo X\n", tms, tdi);
}

unsigned char cable_dlc5_clockout()
{
	unsigned char data;
	
	parallel_writedata((1 << DLC5_PROG) | (0 << DLC5_TCK));
	data = parallel_readstatus();
	data ^= 0x80;
	data >>= DLC5_TDO;
	data &= 1;
	if (globdbg) printf("0: tms X tdi X tdo %d\n", data);

	return data;
}
