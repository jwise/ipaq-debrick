/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <bonobo.h>
#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

static GnomeUIInfo gtagmenu_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("_Scan JTAG Chain"),
    N_("Scans the JTAG chain for devices."),
    (gpointer) redetect_chain, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_MENU_EXIT_ITEM (gtk_main_quit, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo settings1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_PREFERENCES_ITEM (on_preferences_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_ABOUT_ITEM (show_about, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] =
{
  {
    GNOME_APP_UI_SUBTREE, N_("_GTAG"),
    NULL,
    gtagmenu_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_MENU_SETTINGS_TREE (settings1_menu_uiinfo),
  GNOMEUIINFO_MENU_HELP_TREE (help1_menu_uiinfo),
  GNOMEUIINFO_END
};

GtkWidget*
create_gtagmain (void)
{
  GtkWidget *gtagmain;
  GtkWidget *bonobodock1;
  GtkWidget *vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *chainlist;
  GtkWidget *appbar1;

  gtagmain = gnome_app_new ("GTAG", _("GTAG"));
  gtk_window_set_default_size (GTK_WINDOW (gtagmain), 303, 227);

  bonobodock1 = GNOME_APP (gtagmain)->dock;
  gtk_widget_show (bonobodock1);

  gnome_app_create_menus (GNOME_APP (gtagmain), menubar1_uiinfo);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gnome_app_set_contents (GNOME_APP (gtagmain), vbox1);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  chainlist = gtk_tree_view_new ();
  gtk_widget_show (chainlist);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), chainlist);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (chainlist), TRUE);
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (chainlist), TRUE);

  appbar1 = gnome_appbar_new (TRUE, TRUE, GNOME_PREFERENCES_NEVER);
  gtk_widget_show (appbar1);
  gnome_app_set_statusbar (GNOME_APP (gtagmain), appbar1);

  g_signal_connect ((gpointer) gtagmain, "destroy",
                    G_CALLBACK (gtk_main_quit),
                    NULL);
  gnome_app_install_menu_hints (GNOME_APP (gtagmain), menubar1_uiinfo);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (gtagmain, gtagmain, "gtagmain");
  GLADE_HOOKUP_OBJECT (gtagmain, bonobodock1, "bonobodock1");
  GLADE_HOOKUP_OBJECT (gtagmain, menubar1_uiinfo[0].widget, "gtagmenu");
  GLADE_HOOKUP_OBJECT (gtagmain, gtagmenu_menu_uiinfo[0].widget, "redetect");
  GLADE_HOOKUP_OBJECT (gtagmain, gtagmenu_menu_uiinfo[1].widget, "quit1");
  GLADE_HOOKUP_OBJECT (gtagmain, menubar1_uiinfo[1].widget, "settings1");
  GLADE_HOOKUP_OBJECT (gtagmain, settings1_menu_uiinfo[0].widget, "preferences1");
  GLADE_HOOKUP_OBJECT (gtagmain, menubar1_uiinfo[2].widget, "help1");
  GLADE_HOOKUP_OBJECT (gtagmain, help1_menu_uiinfo[0].widget, "about1");
  GLADE_HOOKUP_OBJECT (gtagmain, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (gtagmain, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (gtagmain, chainlist, "chainlist");
  GLADE_HOOKUP_OBJECT (gtagmain, appbar1, "appbar1");

  return gtagmain;
}

GtkWidget*
create_gtagabout (void)
{
  const gchar *authors[] = {
    "Joshua Wise <joshua@joshuawise.com>",
    NULL
  };
  const gchar *documenters[] = { NULL };
  /* TRANSLATORS: Replace this string with your names, one name per line. */
  gchar *translators = _("translator_credits");
  GtkWidget *gtagabout;

  if (!strcmp (translators, "translator_credits"))
    translators = NULL;
  gtagabout = gnome_about_new ("GTAG", VERSION,
                        _("Copyright (C) 2004 Joshua Wise"),
                        _("A JTAG application for GNOME"),
                        authors,
                        documenters,
                        translators,
                        NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (gtagabout, gtagabout, "gtagabout");

  return gtagabout;
}

static GnomeUIInfo menu1_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Passive: Parallel"),
    NULL,
    (gpointer) set_tap_passive_parallel, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo menu2_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Xilinx DLC5 (or compatible)"),
    NULL,
    (gpointer) set_parallel_dlc5, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

static GnomeUIInfo menu3_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("PPDev: /dev/parport0"),
    NULL,
    (gpointer) on_parport0_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("PPDev: /dev/parport1"),
    NULL,
    (gpointer) on_parport1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("IOPerm: 0x378 (LPT1)"),
    NULL,
    (gpointer) on_ioperm378_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_ITEM, N_("IOPerm: 0x278 (LPT2)"),
    NULL,
    (gpointer) on_ioperm278_activate, NULL, NULL,
    GNOME_APP_PIXMAP_NONE, NULL,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};

GtkWidget*
create_gtagsettings (void)
{
  GtkWidget *gtagsettings;
  GtkWidget *notebook1;
  GtkWidget *vbox2;
  GtkWidget *interfacehbox;
  GtkWidget *tapinterfacelabel;
  GtkWidget *taptypemenu;
  GtkWidget *menu1;
  GtkWidget *interfacelabel;
  GtkWidget *ppproptable;
  GtkWidget *pppinoutlbl;
  GtkWidget *ppparinterface;
  GtkWidget *pppinout;
  GtkWidget *menu2;
  GtkWidget *parinterface;
  GtkWidget *menu3;
  GtkWidget *PassiveParallelPropLabel;

  gtagsettings = gnome_property_box_new ();
  gtk_window_set_title (GTK_WINDOW (gtagsettings), _("GTAG Settings"));
  gtk_window_set_resizable (GTK_WINDOW (gtagsettings), FALSE);

  notebook1 = GNOME_PROPERTY_BOX (gtagsettings)->notebook;
  gtk_widget_show (notebook1);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);

  interfacehbox = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (interfacehbox);
  gtk_box_pack_start (GTK_BOX (vbox2), interfacehbox, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (interfacehbox), 3);

  tapinterfacelabel = gtk_label_new (_("TAP Interface"));
  gtk_widget_show (tapinterfacelabel);
  gtk_box_pack_start (GTK_BOX (interfacehbox), tapinterfacelabel, FALSE, FALSE, 0);

  taptypemenu = gtk_option_menu_new ();
  gtk_widget_show (taptypemenu);
  gtk_box_pack_start (GTK_BOX (interfacehbox), taptypemenu, TRUE, TRUE, 0);

  menu1 = gtk_menu_new ();
  gnome_app_fill_menu (GTK_MENU_SHELL (menu1), menu1_uiinfo,
                       NULL, FALSE, 0);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (taptypemenu), menu1);

  interfacelabel = gtk_label_new (_("TAP Interface"));
  gtk_widget_show (interfacelabel);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), interfacelabel);

  ppproptable = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (ppproptable);
  gtk_container_add (GTK_CONTAINER (notebook1), ppproptable);
  gtk_container_set_border_width (GTK_CONTAINER (ppproptable), 3);
  gtk_table_set_row_spacings (GTK_TABLE (ppproptable), 3);
  gtk_table_set_col_spacings (GTK_TABLE (ppproptable), 3);

  pppinoutlbl = gtk_label_new (_("Pinout"));
  gtk_widget_show (pppinoutlbl);
  gtk_table_attach (GTK_TABLE (ppproptable), pppinoutlbl, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (pppinoutlbl), 0, 0.5);

  ppparinterface = gtk_label_new (_("Parallel interface"));
  gtk_widget_show (ppparinterface);
  gtk_table_attach (GTK_TABLE (ppproptable), ppparinterface, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (ppparinterface), 0, 0.5);

  pppinout = gtk_option_menu_new ();
  gtk_widget_show (pppinout);
  gtk_table_attach (GTK_TABLE (ppproptable), pppinout, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu2 = gtk_menu_new ();
  gnome_app_fill_menu (GTK_MENU_SHELL (menu2), menu2_uiinfo,
                       NULL, FALSE, 0);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (pppinout), menu2);

  parinterface = gtk_option_menu_new ();
  gtk_widget_show (parinterface);
  gtk_table_attach (GTK_TABLE (ppproptable), parinterface, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu3 = gtk_menu_new ();
  gnome_app_fill_menu (GTK_MENU_SHELL (menu3), menu3_uiinfo,
                       NULL, FALSE, 0);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (parinterface), menu3);

  PassiveParallelPropLabel = gtk_label_new (_("Passive (Parallel)"));
  gtk_widget_show (PassiveParallelPropLabel);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), PassiveParallelPropLabel);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (gtagsettings, gtagsettings, "gtagsettings");
  GLADE_HOOKUP_OBJECT_NO_REF (gtagsettings, notebook1, "notebook1");
  GLADE_HOOKUP_OBJECT (gtagsettings, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (gtagsettings, interfacehbox, "interfacehbox");
  GLADE_HOOKUP_OBJECT (gtagsettings, tapinterfacelabel, "tapinterfacelabel");
  GLADE_HOOKUP_OBJECT (gtagsettings, taptypemenu, "taptypemenu");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu1_uiinfo[0].widget, "mnupassiveparallel");
  GLADE_HOOKUP_OBJECT (gtagsettings, interfacelabel, "interfacelabel");
  GLADE_HOOKUP_OBJECT (gtagsettings, ppproptable, "ppproptable");
  GLADE_HOOKUP_OBJECT (gtagsettings, pppinoutlbl, "pppinoutlbl");
  GLADE_HOOKUP_OBJECT (gtagsettings, ppparinterface, "ppparinterface");
  GLADE_HOOKUP_OBJECT (gtagsettings, pppinout, "pppinout");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu2_uiinfo[0].widget, "dlc5");
  GLADE_HOOKUP_OBJECT (gtagsettings, parinterface, "parinterface");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu3, "menu3");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu3_uiinfo[0].widget, "parport0");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu3_uiinfo[1].widget, "parport1");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu3_uiinfo[2].widget, "ioperm378");
  GLADE_HOOKUP_OBJECT (gtagsettings, menu3_uiinfo[3].widget, "ioperm278");
  GLADE_HOOKUP_OBJECT (gtagsettings, PassiveParallelPropLabel, "PassiveParallelPropLabel");

  return gtagsettings;
}
