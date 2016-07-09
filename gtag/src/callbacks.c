#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "gtag.h"
#include "ppdev.h"

void
redetect_chain                         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int i;
	int bits=0;
	int ones=0;
	int irlen;
	unsigned int idcode;
	
	jtag_reset();
	jtag_idle();
	jtag_capture_ir();	/* this is not strictly necessary, but it is a good idea */
	while(ones < 8)
	{
		i = jtag_shift_ir(1);
		if (i == 1)
			ones++;
		else {
			bits += ones + 1;
			ones = 0;
		}
	}
	printf("Total IR length: %d bits\n", irlen=bits);	/* We just detected the IR length AND we put all the components in bypass mode! Not bad! */
	
	jtag_capture_dr();	/* again not strictly necessary - the TAP should be able to figure it out anyway, but still not a bad idea */
	bits = 0;
	i = 0;
	while (!i && bits < 32)
	{
		i = jtag_shift_dr(1);
		if (!i)
			bits++;
	}
	printf("Total chain length: %d bits\n", bits);
	
	jtag_capture_ir();
	/* ok, now we will IDCODE the first on the JTAG chain. */
	jtag_shift_ir(0);	/* we need to put us into shift_ir mode first to start data pumping */
	for (i=0; i<irlen-2; i++)
		jtag_shift_ir(1);
	jtag_exit1_ir(1);
	
	jtag_capture_dr();
	idcode = 0;
	for (bits=0; bits<32; bits++)
	{
		idcode <<= 1;
		if (jtag_shift_dr(1))
			idcode |= 0x1;
	}
	printf("%08X\n", idcode);
}

void
show_about                             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_widget_show(create_gtagabout());
}


void
on_ioperm378_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_warning("IOPERM is not supported!");
}


void
on_preferences_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_widget_show(create_gtagsettings());
}


void
on_ioperm278_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_warning("IOPERM is not supported!");
}


void
set_tap_passive_parallel               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	tap_type 		= TAP_PASSIVE;
	passive_interface 	= PASSIVE_INTERFACE_PARALLEL;
}

void
set_parallel_dlc5                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	passive_pinout		= PASSIVE_PINOUT_DLC5;
}

void
on_parport0_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	passive_parallel	= PASSIVE_PARALLEL_PPDEV;
	parallel_ppdev_close();
	parallel_ppdev_open(0);
}


void
on_parport1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	passive_parallel	= PASSIVE_PARALLEL_PPDEV;
	parallel_ppdev_close();
	parallel_ppdev_open(1);
}

