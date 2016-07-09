extern void put_chain(unsigned char *chain, unsigned long pos, unsigned long value);
extern void zero_chain(unsigned char *chain, unsigned long len);
extern void set_guard(unsigned char *chain);
extern void write_chain(unsigned char *chain, unsigned long len);
extern unsigned long get_long_from_chain(unsigned char *chain, unsigned long pos);
extern void read_chain(unsigned char *chain, unsigned long len);
extern void jtag_start(void);

