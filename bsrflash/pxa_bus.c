#include "bus.h"
#include "libjtag.h"
#include "jtag_core.h"

#define PXA_MAX_CHAIN_LEN 410

struct chain_info {
	int len;
	int n_oe_out;
	int n_cs_0_ctrl;
	int *n_cs_out;
	int nr_cs;
	int rd_n_wr_out;
	int n_we_ctrl;
	int n_we_out;
	int md_upper_ctrl;
	int md_lower_ctrl;
	int md_i_base;
	int md_o_base;
	int ma_base;
	int *guard_value;
	int nr_guard_values;
};

static int pxa_250_guard[] = {15, 33, 48, 49, 52, 53, 78, 79, 80, 101, 119, 134, 135, 138, 139, 164, 165, 166, 205, 206, 207, 208, 209, 218, 219, 220, 221, 226, 227, 228, 229, 230, 379, 380};
static int pxa_250_cs[] = {230, 101, 164, 165, 166, 119};

static struct chain_info chain_pxa_250 = {
	.len = 385,
	.n_oe_out = 229,
	.n_cs_0_ctrl = 206,
	.n_cs_out = pxa_250_cs,
	.nr_cs = 6,
	.rd_n_wr_out = 205,
	.n_we_ctrl = 207,
	.n_we_out = 228,
	.md_upper_ctrl = 84,
	.md_lower_ctrl = 85,
	.md_i_base = 344,
	.md_o_base = 173,
	.ma_base = 231,
	.guard_value = pxa_250_guard,
	.nr_guard_values = sizeof(pxa_250_guard)/sizeof(int),
};

static int pxa_255_guard[] = {245, 224, 226, 227, 234, 235, 359, 360, 242, 243, 88, 225, 244, 245, 246, 405, 404};
static int pxa_255_cs[] = {246};

static struct chain_info chain_pxa_255 = {
	.len = 410,
	.n_oe_out = 245,     //225
	.n_cs_0_ctrl = 224,
	.n_cs_out = pxa_255_cs,
	.nr_cs = 1,
	.rd_n_wr_out = 88, // 183
	.n_we_ctrl = 225,    
	.n_we_out = 244,
	.md_upper_ctrl = 93,
	.md_lower_ctrl = 94,
	.md_i_base = 369,
	.md_o_base = 191,
	.ma_base = 247,
	.guard_value = pxa_255_guard,
	.nr_guard_values = sizeof(pxa_255_guard)/sizeof(int),
};

static unsigned char ichain[(PXA_MAX_CHAIN_LEN/8)+1];
static unsigned char ochain[(PXA_MAX_CHAIN_LEN/8)+1];

#define  PXA_CHAIN_LEN    (chip->len)
#define  nOE_OUT          (chip->n_oe_out)
#define  nCS_0_CTRL       (chip->n_cs_0_ctrl)
#define  nCS              (chip->n_cs_out)
#define  RDnWR_OUT        (chip->rd_n_wr_out)
#define  nWE_CTRL         (chip->n_we_ctrl)
#define  nWE_OUT          (chip->n_we_out)
#define  MDUPPER_CTRL     (chip->md_upper_ctrl)
#define  MDLOWER_CTRL     (chip->md_lower_ctrl)
#define  ADDR_BASE        (chip->ma_base)
#define  DATA_I_BASE      (chip->md_i_base)
#define  DATA_O_BASE      (chip->md_o_base)

#define SET_PIN(p, s)   put_chain(ochain, p, s)

static struct chain_info *chip = 0;

static void pxa_guard(unsigned char *chain) {
	int i;
	for(i = 0 ; i < chip->nr_guard_values ; i++)
		SET_PIN(chip->guard_value[i], 1);
}

static unsigned long pxa_bus_read(unsigned long addr) {
	int i;

//	set control pins
	SET_PIN(nOE_OUT, 1);       //Enable outputs.
	SET_PIN(nWE_OUT,  1);      //Disable writing.
	SET_PIN(RDnWR_OUT, 1);     //Read mode

//	set data bus HiZ
	SET_PIN(MDUPPER_CTRL, 0);
	SET_PIN(MDLOWER_CTRL, 0);

//	set ship selects
	for(i = 0 ; i < chip->nr_cs ; i++)
		SET_PIN(nCS[i], 1);
// SETUP
	for(i = 0 ; i <= 25 ; i++)
		put_chain(ochain, i + ADDR_BASE, (addr >> i) & 1);
	write_chain(ochain, PXA_CHAIN_LEN);

// READ
	SET_PIN(nCS[0], 0);
	SET_PIN(nOE_OUT, 0);
	write_chain(ochain, PXA_CHAIN_LEN);

// READ
	read_chain(ichain, PXA_CHAIN_LEN);
	return get_long_from_chain(ichain, DATA_I_BASE);
}

static void pxa_bus_write(unsigned long addr, unsigned long data) {
	int i;

	//      set ship selects
	for(i = 0 ; i < chip->nr_cs ; i++)
		SET_PIN(nCS[i], 1);

//      set control pins
	SET_PIN(nOE_OUT, 1);      
	SET_PIN(nWE_OUT,  1);    
	SET_PIN(RDnWR_OUT, 0); 

//	set data bus to drive
	SET_PIN(MDUPPER_CTRL, 1);
	SET_PIN(MDLOWER_CTRL, 1);

//	set address bus
	for(i = 0 ; i <= 25 ; i++)
		put_chain(ochain, i + ADDR_BASE, (addr >> i) & 1);

//	set data bus value
	for(i = 0 ; i <= 31 ; i++)
		put_chain(ochain, i + DATA_O_BASE, (data >> i) & 1);

// SETUP
	write_chain(ochain, PXA_CHAIN_LEN);
	
	SET_PIN(nCS[0], 0);
	SET_PIN(nWE_OUT,  0);

// WRITE
	write_chain(ochain, PXA_CHAIN_LEN);

	SET_PIN(nOE_OUT, 0);
	SET_PIN(nWE_OUT, 1);
	
//      set data bus to drive
	SET_PIN(MDUPPER_CTRL, 0);
	SET_PIN(MDLOWER_CTRL, 0);

// HOLD
	write_chain(ochain, PXA_CHAIN_LEN);

}

static void pxa_bus_init(void) {
	int chip_nr;

	jtag_start();
	chip_nr = look_for_pxa(); // returns to idle state
	printf("%08x\n", chip_nr);
	switch(chip_nr){
		case 0xc8026496: chip = &chain_pxa_255; break;
		case 0xc8026492: chip = &chain_pxa_250; break;
		default: printf("unsupported PXA chip\n"); exit(0);
	}
	set_instr(PRELOAD);
	zero_chain(ochain, PXA_CHAIN_LEN);
	pxa_guard(ochain);
	write_chain(ochain, PXA_CHAIN_LEN);  // Paranoia ;-)
}

static void pxa_bus_end(void) {
	test_reset();
}

static struct bus pxa_bus = {
	.init       = pxa_bus_init,
	.read_word  = pxa_bus_read,
	.write_word = pxa_bus_write,
	.end        = pxa_bus_end
};

struct bus *pxa_bus_probe(void) {
	return &pxa_bus;
}
