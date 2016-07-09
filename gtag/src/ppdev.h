/* Parallel port device header */
extern void parallel_ppdev_writedata(unsigned char data);
extern unsigned char parallel_ppdev_readstatus();
extern void parallel_ppdev_open(int ppn);
extern void parallel_ppdev_close();
