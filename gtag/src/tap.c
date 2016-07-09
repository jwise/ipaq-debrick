/* TAP C file */
#include <gnome.h>
#include "gtag.h"
#include "dlc5.h"
#include "wiggler.h"
#include "ppdev.h"

int tap_state = STATE_TEST_LOGIC_RESET;

int tap_state_table[][2] =
{
				  /* TMS = 0 */		/* TMS = 1 */
/* STATE_TEST_LOGIC_RESET */	{ STATE_RUN_TEST_IDLE,	STATE_TEST_LOGIC_RESET	},
/* STATE_RUN_TEST_IDLE */	{ STATE_RUN_TEST_IDLE,	STATE_SELECT_DR_SCAN	},
/* STATE_SELECT_DR_SCAN */	{ STATE_CAPTURE_DR,	STATE_SELECT_IR_SCAN	},
/* STATE_CAPTURE_DR */		{ STATE_SHIFT_DR,	STATE_EXIT1_DR		},
/* STATE_SHIFT_DR */		{ STATE_SHIFT_DR,	STATE_EXIT1_DR		},
/* STATE_EXIT1_DR */		{ STATE_PAUSE_DR,	STATE_UPDATE_DR		},
/* STATE_PAUSE_DR */		{ STATE_PAUSE_DR,	STATE_EXIT2_DR		},
/* STATE_EXIT2_DR */		{ STATE_SHIFT_DR,	STATE_UPDATE_DR		},
/* STATE_UPDATE_DR */		{ STATE_RUN_TEST_IDLE,	STATE_SELECT_DR_SCAN	},
/* STATE_SELECT_IR_SCAN */	{ STATE_CAPTURE_IR,	STATE_TEST_LOGIC_RESET	},
/* STATE_CAPTURE_IR */		{ STATE_SHIFT_IR,	STATE_EXIT1_IR		},
/* STATE_SHIFT_IR */		{ STATE_SHIFT_IR,	STATE_EXIT1_IR		},
/* STATE_EXIT1_IR */		{ STATE_PAUSE_IR,	STATE_UPDATE_IR		},
/* STATE_PAUSE_IR */		{ STATE_PAUSE_IR,	STATE_EXIT2_IR		},
/* STATE_EXIT2_IR */		{ STATE_SHIFT_IR,	STATE_UPDATE_IR		},
/* STATE_UPDATE_IR */		{ STATE_RUN_TEST_IDLE,	STATE_SELECT_DR_SCAN	},
};

/* Dummy TAP functionality for when a TAP has not yet been set up. */
int tap_dummy_reset(){};
int tap_dummy_idle(){};
int tap_dummy_select_dr(){};
int tap_dummy_capture_dr(){};
int tap_dummy_shift_dr(int i){};
int tap_dummy_exit1_dr(int i){}
int tap_dummy_pause_dr(){};
int tap_dummy_exit2_dr(){};
int tap_dummy_update_dr(){};
int tap_dummy_select_ir(){};
int tap_dummy_capture_ir(){};
int tap_dummy_shift_ir(int i){};
int tap_dummy_exit1_ir(int i){}
int tap_dummy_pause_ir(){};
int tap_dummy_exit2_ir(){};
int tap_dummy_update_ir(){};

/* Passive TAP functionality. */
int tap_passive_reset();
int tap_passive_idle();
int tap_passive_select_dr();
int tap_passive_capture_dr();
int tap_passive_shift_dr(int i);
int tap_passive_exit1_dr(int i);
int tap_passive_pause_dr();
int tap_passive_exit2_dr();
int tap_passive_update_dr();
int tap_passive_select_ir();
int tap_passive_capture_ir();
int tap_passive_shift_ir(int i);
int tap_passive_exit1_ir(int i);
int tap_passive_pause_ir();
int tap_passive_exit2_ir();
int tap_passive_update_ir();

int tap_ptrs = -1;
int (*tap_reset)() = tap_dummy_reset;
int (*tap_idle)() = tap_dummy_idle;
int (*tap_select_dr)() = tap_dummy_select_dr;
int (*tap_capture_dr)() = tap_dummy_capture_dr;
int (*tap_shift_dr)(int) = tap_dummy_shift_dr;
int (*tap_exit1_dr)(int) = tap_dummy_exit1_dr;
int (*tap_pause_dr)() = tap_dummy_pause_dr;
int (*tap_exit2_dr)() = tap_dummy_exit2_dr;
int (*tap_update_dr)() = tap_dummy_update_dr;
int (*tap_select_ir)() = tap_dummy_select_ir;
int (*tap_capture_ir)() = tap_dummy_capture_ir;
int (*tap_shift_ir)(int) = tap_dummy_shift_ir;
int (*tap_exit1_ir)(int) = tap_dummy_exit1_ir;
int (*tap_pause_ir)() = tap_dummy_pause_ir;
int (*tap_exit2_ir)() = tap_dummy_exit2_ir;
int (*tap_update_ir)() = tap_dummy_update_ir;

inline static void tap_setup_pointers()
{
	if (tap_ptrs != tap_type)
		switch(tap_type)
		{
		case TAP_PASSIVE:
			tap_reset = tap_passive_reset;
			tap_idle = tap_passive_idle;
			tap_select_dr = tap_passive_select_dr;
			tap_capture_dr = tap_passive_capture_dr;
			tap_shift_dr = tap_passive_shift_dr;
			tap_exit1_dr = tap_passive_exit1_dr;
			tap_pause_dr = tap_passive_pause_dr;
			tap_exit2_dr = tap_passive_exit2_dr;
			tap_update_dr = tap_passive_update_dr;
			tap_select_ir = tap_passive_select_ir;
			tap_capture_ir = tap_passive_capture_ir;
			tap_shift_ir = tap_passive_shift_ir;
			tap_exit1_ir = tap_passive_exit1_ir;
			tap_pause_ir = tap_passive_pause_ir;
			tap_exit2_ir = tap_passive_exit2_ir;
			tap_update_ir = tap_passive_update_ir;
			break;
		case TAP_UNKNOWN:
			g_warning("Attempting to set up TAP pointers, but TAPtype is unknown! Using tap_dummy!");
			tap_reset = tap_dummy_reset;
			tap_idle = tap_dummy_idle;
			tap_select_dr = tap_dummy_select_dr;
			tap_capture_dr = tap_dummy_capture_dr;
			tap_shift_dr = tap_dummy_shift_dr;
			tap_exit1_dr = tap_dummy_exit1_dr;
			tap_pause_dr = tap_dummy_pause_dr;
			tap_exit2_dr = tap_dummy_exit2_dr;
			tap_update_dr = tap_dummy_update_dr;
			tap_select_ir = tap_dummy_select_ir;
			tap_capture_ir = tap_dummy_capture_ir;
			tap_shift_ir = tap_dummy_shift_ir;
			tap_exit1_ir = tap_dummy_exit1_ir;
			tap_pause_ir = tap_dummy_pause_ir;
			tap_exit2_ir = tap_dummy_exit2_ir;
			tap_update_ir = tap_dummy_update_ir;
			break;
		default:
			g_warning("I don't know what type of TAP this is!");
			g_error("Aaaaaaaaaaaaaaaargghhh....");
			abort();
		}
	tap_ptrs = tap_type;
}

int jtag_reset()
{
	int ret;
	
	/* We can reset the TAP from any state. */
	tap_setup_pointers();
	
	ret = tap_reset();
	tap_state = STATE_TEST_LOGIC_RESET;	// make sure to FORCE into RESET state! this MIGHT fail but it SHOULDN'T!
	
	return ret;
}

#define DEF_JTAG(x) int jtag_##x () { tap_setup_pointers(); return tap_##x (); }
#define DEF_JTAGI(x) int jtag_##x (int y) { tap_setup_pointers(); return tap_##x (y); }

DEF_JTAG (idle)
DEF_JTAG (select_dr)
DEF_JTAG (capture_dr)
DEF_JTAGI(shift_dr)
DEF_JTAGI(exit1_dr)
DEF_JTAG (pause_dr)
DEF_JTAG (exit2_dr)
DEF_JTAG (update_dr)
DEF_JTAG (select_ir)
DEF_JTAG (capture_ir)
DEF_JTAGI(shift_ir)
DEF_JTAGI(exit1_ir)
DEF_JTAG (pause_ir)
DEF_JTAG (exit2_ir)
DEF_JTAG (update_ir)

#undef  DEF_JTAG
#undef  DEF_JTAGI

/* Passive code can go in this file. Why not? */
int cable_ptrs = -1;		/* which cable are we set up for right now? */
int interface_ptrs = -1;	/* which interface are we set up for right now? */

void		cable_dummy_clockin(int tms, int tdi);
unsigned char	cable_dummy_clockout();

void		parallel_dummy_writedata(unsigned char data);
unsigned char	parallel_dummy_readstatus();


void		(*cable_clockin)(int tms, int tdi)		= cable_dummy_clockin;
unsigned char	(*cable_clockout)()				= cable_dummy_clockout;

void		(*parallel_writedata)(unsigned char data)	= parallel_dummy_writedata;
unsigned char	(*parallel_readstatus)()			= parallel_dummy_readstatus;


inline static void passive_setup_pointers()
{
	if (cable_ptrs != passive_pinout)
		switch(passive_pinout)
		{
		case PASSIVE_PINOUT_UNKNOWN:
			g_warning("Attempting to set up passive pointers, but pinout is unknown! Using cable_dummy!");
			cable_clockin = cable_dummy_clockin;
			cable_clockout = cable_dummy_clockout;
			break;
		case PASSIVE_PINOUT_DLC5:
			cable_clockin = cable_dlc5_clockin;
			cable_clockout = cable_dlc5_clockout;
			break;
		case PASSIVE_PINOUT_WIGGLER:
			cable_clockin = cable_wiggler_clockin;
			cable_clockout = cable_wiggler_clockout;
			break;
		default:
			g_warning("I don't know what type of pinout this is!");
			g_error("Aaaaaarrrggghhh......");
			abort();
		}
	cable_ptrs = passive_pinout;
	if (interface_ptrs != passive_parallel)
		switch(passive_parallel)
		{
		case PASSIVE_PARALLEL_IOPERM:
			g_warning("IOPerm is NYI!");
			g_error("Aaarrghhhh...");
			abort();
		case PASSIVE_PARALLEL_PPDEV:
			parallel_writedata = parallel_ppdev_writedata;
			parallel_readstatus = parallel_ppdev_readstatus;
			break;
		case PASSIVE_PARALLEL_UNKNOWN:
			g_warning("Attempting to set up passive pointers, but parallel is unknown! Using parallel_dummy!");
			parallel_writedata = parallel_dummy_writedata;
			parallel_readstatus = parallel_dummy_readstatus;
			break;
		default:
			g_warning("I don't know what type of parallel port this is!");
			g_error("Aaaaarrrgghhh....");
			abort();
		}
	interface_ptrs = passive_parallel;
}

void cable_dummy_clockin(int tms, int tdi){}
unsigned char cable_dummy_clockout(){return 0;}
void parallel_dummy_writedata(unsigned char data){}
unsigned char parallel_dummy_readstatus(){return 0;}

int tap_passive_reset()
{
	passive_setup_pointers();
	
	/* This is rather easy for passive. */
	cable_clockin(1, 0);
	cable_clockin(1, 0);
	cable_clockin(1, 0);
	cable_clockin(1, 0);
	cable_clockin(1, 0);
	tap_state = STATE_TEST_LOGIC_RESET;
	
	return 0;		/* -1 failure everything else success */
}

int tap_passive_idle()
{
	passive_setup_pointers();
	
	switch (tap_state)
	{
	case STATE_RUN_TEST_IDLE:
	case STATE_TEST_LOGIC_RESET:
		cable_clockin(0, 0);
		tap_state = STATE_RUN_TEST_IDLE;
		return 0;
	case STATE_SELECT_DR_SCAN:
		cable_clockin(1, 0);
		tap_state = STATE_SELECT_IR_SCAN;
	case STATE_SELECT_IR_SCAN:
		cable_clockin(1, 0);
		cable_clockin(0, 0);
		tap_state = STATE_RUN_TEST_IDLE;
		return 0;
	case STATE_CAPTURE_DR:
	case STATE_SHIFT_DR:
	case STATE_EXIT1_DR:
	case STATE_PAUSE_DR:
	case STATE_EXIT2_DR:
		if (jtag_update_dr() < 0)
			return -1;
	case STATE_UPDATE_DR:
		cable_clockin(0, 0);
		tap_state = STATE_RUN_TEST_IDLE;
		return 0;
	case STATE_CAPTURE_IR:
	case STATE_SHIFT_IR:
	case STATE_EXIT1_IR:
	case STATE_PAUSE_IR:
	case STATE_EXIT2_IR:
		if (jtag_update_ir() < 0)
			return -1;
	case STATE_UPDATE_IR:
		cable_clockin(0, 0);
		tap_state = STATE_RUN_TEST_IDLE;
		return 0;
	default:
		g_error("Unknown state for tap_passive_idle");
		abort();
	}
	return 0;
}

int tap_passive_select_dr()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_SELECT_DR_SCAN)
		return 0;
	if (tap_state != STATE_RUN_TEST_IDLE)
		if (jtag_idle() < 0)
			return -1;
	cable_clockin(1, 0);
	tap_state = STATE_SELECT_DR_SCAN;
	return 0;
}

int tap_passive_capture_dr()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_CAPTURE_DR)
		return 0;
	if (tap_state != STATE_SELECT_DR_SCAN)
		if (jtag_select_dr() < 0) return -1;
	cable_clockin(0, 0);
	tap_state = STATE_CAPTURE_DR;
	return 0;
}

int tap_passive_shift_dr(int i)
{
	passive_setup_pointers();
	
	if ((tap_state != STATE_SHIFT_DR) && (tap_state != STATE_CAPTURE_DR) && (tap_state != STATE_EXIT2_DR))
		if (jtag_capture_dr() < 0)
			return -1;
	cable_clockin(0, i ? 1 : 0);
	tap_state = STATE_SHIFT_DR;
	return cable_clockout();
}

int tap_passive_exit1_dr(int i)
{
	passive_setup_pointers();
	
	if (tap_state == STATE_EXIT1_DR)
		return 0;
	if ((tap_state != STATE_CAPTURE_DR) && (tap_state != STATE_SHIFT_DR))
		if (jtag_capture_dr() < 0)
			return -1;
	cable_clockin(1, i?1:0);
	tap_state = STATE_EXIT1_DR;
	return 0;
}

int tap_passive_pause_dr()
{
	passive_setup_pointers();
	
	if ((tap_state != STATE_EXIT1_DR) && (tap_state != STATE_PAUSE_DR))
		if (jtag_exit1_dr(0) < 0)
			return -1;
	cable_clockin(0, 0);
	tap_state = STATE_PAUSE_DR;
	return 0;
}

int tap_passive_exit2_dr()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_EXIT2_DR)
		return 0;
	if (tap_state != STATE_PAUSE_DR)
		if (jtag_pause_dr() < 0)
			return -1;
	cable_clockin(1, 0);
	tap_state = STATE_EXIT2_DR;
	return 0;
}

int tap_passive_update_dr()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_UPDATE_DR)
		return 0;
	if ((tap_state != STATE_EXIT1_DR) && (tap_state != STATE_EXIT2_DR))
	{
		if (tap_state == STATE_PAUSE_DR)
		{
			if (jtag_exit2_dr() < 0)
				return -1;
		} else
			if (jtag_exit1_dr(1) < 0)
				return -1;
	}
	/* we are now in either exit1 or exit2 */
	cable_clockin(1, 0);
	return 0;
}

int tap_passive_select_ir()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_SELECT_IR_SCAN)
		return 0;
	if (jtag_select_dr() < 0)
		return -1;
	cable_clockin(1, 0);
	tap_state = STATE_SELECT_IR_SCAN;
	return 0;
}

int tap_passive_capture_ir()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_CAPTURE_IR)
		return 0;
	if (tap_state != STATE_SELECT_IR_SCAN)
		if (jtag_select_ir() < 0)
			return -1;
	cable_clockin(0, 0);
	tap_state = STATE_CAPTURE_IR;
	return 0;
}

int tap_passive_shift_ir(int i)
{
	passive_setup_pointers();
	
	if ((tap_state != STATE_SHIFT_IR) && (tap_state != STATE_CAPTURE_IR) && (tap_state != STATE_EXIT2_IR))
		if (jtag_capture_ir() < 0)
			return -1;
	cable_clockin(0, i ? 1 : 0);
	tap_state = STATE_SHIFT_IR;
	return cable_clockout();
}

int tap_passive_exit1_ir(int i)
{
	passive_setup_pointers();
	
	if (tap_state == STATE_EXIT1_IR)
		return 0;
	if ((tap_state != STATE_CAPTURE_IR) && (tap_state != STATE_SHIFT_IR))
		if (jtag_capture_ir() < 0)
			return -1;
	cable_clockin(1, i ? 1 : 0);
	tap_state = STATE_EXIT1_IR;
	return 0;
}

int tap_passive_pause_ir()
{
	passive_setup_pointers();
	
	if ((tap_state != STATE_EXIT1_IR) && (tap_state != STATE_PAUSE_IR))
		if (jtag_exit1_ir(1) < 0)
			return -1;
	cable_clockin(0, 0);
	tap_state = STATE_PAUSE_IR;
	return 0;
}

int tap_passive_exit2_ir()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_EXIT2_IR)
		return 0;
	if (tap_state != STATE_PAUSE_IR)
		if (jtag_pause_ir() < 0)
			return -1;
	cable_clockin(1, 0);
	tap_state = STATE_EXIT2_IR;
	return 0;
}

int tap_passive_update_ir()
{
	passive_setup_pointers();
	
	if (tap_state == STATE_UPDATE_IR)
		return 0;
	if ((tap_state != STATE_EXIT1_IR) && (tap_state != STATE_EXIT2_IR))
	{
		if (tap_state == STATE_PAUSE_IR)
		{
			if (jtag_exit2_ir() < 0)
				return -1;
		} else
			if (jtag_exit1_ir(1) < 0)
				return -1;
	}
	/* we are now in either exit1 or exit2 */
	cable_clockin(1, 0);
	return 0;
}
