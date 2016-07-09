#ifndef __jtag_core_h__
#define __jtag_core_h__

extern void put_chain(unsigned char *chain, unsigned long pos, unsigned long value);
extern void zero_chain(unsigned char *chain, unsigned long len);
extern void one_chain(unsigned char *chain, unsigned long len);
extern void set_guard(unsigned char *chain);
extern void write_chain(unsigned char *chain, unsigned long len);
extern unsigned long get_long_from_chain(unsigned char *chain, unsigned long pos);
extern void read_chain(unsigned char *chain, unsigned long len);
extern void jtag_start(void);
extern void chain_through(unsigned char* ichain, unsigned char* ochain, unsigned long len);
extern void setup_chain_with_flags(unsigned char* chain, int length);
extern void nand_data_to_chain(unsigned char* chain, unsigned char data);
extern unsigned char nand_data_from_chain(unsigned char* chain);

#endif
