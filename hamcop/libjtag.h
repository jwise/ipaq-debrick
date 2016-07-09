#ifndef __libjtag_h__
#define __libjtag_h__

#ifndef WIGGLER
#define TDI     0
#define TCK     1
#define TMS     2
#define TDO     4
#else	/* WIGGLER */
#define TDI	3
#define TCK	2
#define TMS	1
#define nTRST	0
#define TDO	7
#endif
#define PROG	TDO

#define EXTEST		0x02
#define SAMPLE          0x00
#define PRELOAD		0x00
#define BYPASS		0x3F
#define IDCODE		0x3D

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

#endif
