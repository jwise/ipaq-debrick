#ifndef __hamcop_h__
#define __hamcop_h__

#define HAMCOP_CHAIN_LEN 619
#define HAMCOP_LED_ENABLE 69
#define HAMCOP_LED_AMBER 38
#define HAMCOP_LED_GREEN 36
#define HAMCOP_LED_BLUE 33

#define SET_PIN(p, s)   put_chain(ochain, p, s)

#define HAMCOP_NAND_IO_HIGHZ 142
#define HAMCOP_NAND_IO_7_OUT 127
#define HAMCOP_NAND_IO_6_OUT 129
#define HAMCOP_NAND_IO_5_OUT 131
#define HAMCOP_NAND_IO_4_OUT 133
#define HAMCOP_NAND_IO_3_OUT 135
#define HAMCOP_NAND_IO_2_OUT 137
#define HAMCOP_NAND_IO_1_OUT 139
#define HAMCOP_NAND_IO_0_OUT 141
#define HAMCOP_NAND_IO_7_IN 129
#define HAMCOP_NAND_IO_6_IN 131
#define HAMCOP_NAND_IO_5_IN 133
#define HAMCOP_NAND_IO_4_IN 135
#define HAMCOP_NAND_IO_3_IN 137
#define HAMCOP_NAND_IO_2_IN 139
#define HAMCOP_NAND_IO_1_IN 141
#define HAMCOP_NAND_IO_0_IN 144
#define HAMCOP_NAND_READY 98
#define HAMCOP_NAND_nRE 99
#define HAMCOP_NAND_nCE 100
#define HAMCOP_NAND_nWE 98
#define HAMCOP_NAND_nWP 96
#define HAMCOP_NAND_CLE 102
#define HAMCOP_NAND_ALE 101

#define LED_BLUE 0x1
#define LED_AMBER 0x2
#define LED_GREEN 0x4

struct device_table {
    int manufacturer_code;
    int device_code;
    char *manufacturer_name;
    char *device_name;
};

extern unsigned char ichain[HAMCOP_CHAIN_LEN/8+1];
extern unsigned char ochain[(HAMCOP_CHAIN_LEN/8)+1];
extern int chain_flags;
extern struct device_table devids[];
extern char *get_device_name(int device_code);
extern char *get_manufacturer_name(int manufacturer_code);

#endif
