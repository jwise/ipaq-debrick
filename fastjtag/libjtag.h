#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/ppdev.h>

#ifndef WIGGLER
#define TDI     0
#define TCK     1
#define TMS     2
#define CTRL    3
#define TDO     4
#else	/* WIGGLER */
#define TDI	3
#define TCK	2
#define TMS	1
#define nTRST	0
#define TDO	7
#endif
#define PROG	TDO

#define EXTEST		0x00
#define SAMPLEPRELOAD	0x01
#define BYPASS		0x1F
#define CLAMP		0x04
#define HIGHZ		0x08
#define IDCODE		0x1E
#define DBGRX		0x02
#define LDIC		0x07
#define SELDCSR		0x09
#define DBGTX		0x10

extern void openport(void);
extern void closeport(void);
extern void clockin(int tms, int tdi);
extern void clock(void);
extern unsigned char clockout(void);
extern void test_reset();
extern void capture_dr();
extern void capture_ir();
extern unsigned int clock_bits_out(int bitc);
extern unsigned int clock_bits_out_rev(int bitc);
extern void set_instr(int instr);
extern void look_for_pxa();
extern int gettxword(unsigned long* wordptr);
extern void sendtxword(unsigned long txword);
extern void jstartfastread(unsigned long addr, unsigned long words);
extern void jstartfastwrite();
extern void jstartfastwriteh();
extern unsigned long jfastread();
extern unsigned long jread(unsigned long addr);
extern unsigned long jreadh(unsigned long addr);
extern int jcheck();
extern unsigned long jwrite(unsigned long addr, unsigned long value);
extern unsigned long jwriteh(unsigned long addr, unsigned long value);
extern unsigned long jfastwrite(unsigned long addr, unsigned long value);
