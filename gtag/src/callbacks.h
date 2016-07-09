#include <gnome.h>


void
redetect_chain                         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
show_about                             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_mnupassiveparallel_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
set_parallel_dlc5                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_parport0_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_parport1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ioperm378_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ioperm278_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
set_tap_passive_parallel               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
