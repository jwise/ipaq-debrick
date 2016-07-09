#ifndef __nand_h__
#define __nand_h__

/* All these values are in nanoseconds */

#define NAND_WE_HOLD_TIME 30 /* min = 15, max = - */
#define NAND_WRITE_PULSE_TIME 50 /* min = 25, max = - */
#define NAND_READ_PULSE_TIME 70 /* min = 35, max = - */
#define NAND_ALE_SETUP_TIME 10 /* min = 0, max = - */
#define NAND_READ_ACCESS_TIME 35 /* min = -, max = 35 */

#define NAND_CMD_READA  0x00
#define NAND_CMD_READB  0x01
#define NAND_CMD_PROG2  0x10
#define NAND_CMD_READC  0x50
#define NAND_CMD_ERASE1 0x60
#define NAND_CMD_STATUS 0x70
#define NAND_CMD_PROG1  0x80
#define NAND_CMD_READID 0x90
#define NAND_CMD_ERASE2 0xD0
#define NAND_CMD_RESET  0xFF

#define NAND_PAGE_SIZE 512
#define NAND_OOB_SIZE 16
#define NAND_BLOCK_SIZE 32 /* In pages */
#define NAND_PROG_TIME 500 /* microseconds */
#define NAND_ERASE_TIME 3000 /* microseconds */

#define NANOSLEEP(x) { \
			struct timespec req = {0, x}; \
			struct timespec rem = {0, 0}; \
			nanosleep(&req, &rem); \
		     }

extern void nand_sane(void);
extern void nand_command(unsigned char cmd);
extern void nand_address(unsigned char addr);
extern unsigned char nand_read(char* descr);
extern void nand_write(unsigned char d);
extern int nand_ready(void);

#endif
