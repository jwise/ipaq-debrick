#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "interface.h"
#include "support.h"
#include "ppdev.h"
#include "gtag.h"

int tap_type = TAP_PASSIVE;
int passive_interface = PASSIVE_INTERFACE_PARALLEL;
int passive_pinout = PASSIVE_PINOUT_WIGGLER;
int passive_parallel = PASSIVE_PARALLEL_PPDEV;
int globdbg = 0;

GtkWidget *gtag_chainlist;

int
main (int argc, char *argv[])
{
  GtkWidget *gtagmain;
  GtkTreeViewColumn *col;
  GtkCellRenderer *renderer;
  GtkTreeStore *store;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif
  globdbg = argc - 1;	/* how cheap */

  gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE,
                      argc, argv,
                      GNOME_PARAM_APP_DATADIR, PACKAGE_DATA_DIR,
                      NULL);
  parallel_ppdev_open(0);
  redetect_chain();
  return;

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  gtagmain = create_gtagmain ();
  gtag_chainlist = lookup_widget(gtagmain, "chainlist");
  
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "#");
  gtk_tree_view_append_column(GTK_TREE_VIEW(gtag_chainlist), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", 0);
  
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Manufacturer");
  gtk_tree_view_append_column(GTK_TREE_VIEW(gtag_chainlist), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", 1);
  
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Part");
  gtk_tree_view_append_column(GTK_TREE_VIEW(gtag_chainlist), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", 2);
  
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Stepping");
  gtk_tree_view_append_column(GTK_TREE_VIEW(gtag_chainlist), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", 3);
  
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Instruction");
  gtk_tree_view_append_column(GTK_TREE_VIEW(gtag_chainlist), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", 4);
  
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "Register");
  gtk_tree_view_append_column(GTK_TREE_VIEW(gtag_chainlist), col);
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, "text", 5);
  
  store = gtk_tree_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(gtag_chainlist), GTK_TREE_MODEL(store));
  g_object_unref(store);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(gtag_chainlist)), GTK_SELECTION_NONE);
  
  gtk_widget_show (gtagmain);
  
  gtk_main ();
  return 0;
}

