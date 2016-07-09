/* gtag header */

enum taptypes {
	TAP_UNKNOWN = 0,
	TAP_PASSIVE,
};

enum passiveinterfaces {
	PASSIVE_INTERFACE_UNKNOWN = 0,
	PASSIVE_INTERFACE_PARALLEL,
};

enum passivepinouts {
	PASSIVE_PINOUT_UNKNOWN = 0,
	PASSIVE_PINOUT_DLC5,
	PASSIVE_PINOUT_WIGGLER
};

enum passiveparallels {
	PASSIVE_PARALLEL_UNKNOWN = 0,
	PASSIVE_PARALLEL_PPDEV,
	PASSIVE_PARALLEL_IOPERM
};

extern int tap_type;
extern int passive_interface;
extern int passive_pinout;
extern int passive_parallel;
extern int globdbg; /* global debugging level */

enum tapstates {
	STATE_TEST_LOGIC_RESET,
	STATE_RUN_TEST_IDLE,
	STATE_SELECT_DR_SCAN,
	STATE_CAPTURE_DR,
	STATE_SHIFT_DR,
	STATE_EXIT1_DR,
	STATE_PAUSE_DR,
	STATE_EXIT2_DR,
	STATE_UPDATE_DR,
	STATE_SELECT_IR_SCAN,
	STATE_CAPTURE_IR,
	STATE_SHIFT_IR,
	STATE_EXIT1_IR,
	STATE_PAUSE_IR,
	STATE_EXIT2_IR,
	STATE_UPDATE_IR,
};

extern int tap_state;
extern int tap_state_table[][2];

extern void		(*cable_clockin)(int tms, int tdi);
extern unsigned char	(*cable_clockout)();

extern void		(*parallel_writedata)(unsigned char data);
extern unsigned char	(*parallel_readstatus)();

extern int jtag_reset();
extern int jtag_idle();
extern int jtag_select_dr();
extern int jtag_capture_dr();
extern int jtag_shift_dr(int);
extern int jtag_exit1_dr();
extern int jtag_pause_dr();
extern int jtag_exit2_dr();
extern int jtag_update_dr();
extern int jtag_select_ir();
extern int jtag_capture_ir();
extern int jtag_shift_ir(int);
extern int jtag_exit1_ir();
extern int jtag_pause_ir();
extern int jtag_exit2_ir();
extern int jtag_update_ir();
