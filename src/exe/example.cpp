#include<gtk/gtk.h>
int main(int argc,
         char *argv[]) {
     GtkWidget *window;
     GtkWidget *grid;
     GtkWidget *menubar;
     GtkWidget *filemenu;
     GtkWidget *quit;
     GtkAccelGroup *accel_group = NULL;

     gtk_init(&argc,&argv);
     window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title(GTK_WINDOW(window), "Try Grid");
     gtk_container_set_border_width(GTK_CONTAINER(window), 05);

     grid = gtk_grid_new();
     gtk_container_add(GTK_CONTAINER(window), grid);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid),TRUE);
     menubar = gtk_menu_bar_new();
     filemenu = gtk_menu_new();

     accel_group = gtk_accel_group_new();
     gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

     GtkWidget *file = gtk_menu_item_new_with_mnemonic("_File");
     quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,
accel_group);

     gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
     gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
     gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

     g_signal_connect(G_OBJECT(quit), "activate",
G_CALLBACK(gtk_main_quit), NULL);

     gtk_grid_attach(GTK_GRID(grid), menubar, 0, 0, 2, 1);


     /* Color Scheme */
     GdkColor colorRed2 = {0x0000, 65535, 29555, 24158};
     GdkColor colorBlue = {0x0000, 3598, 57054, 61937};
     GdkColor colorWht = {0x0000, 65535, 65535, 65535};


     GtkWidget *Abutton = gtk_button_new_with_label("A");
     gtk_grid_attach(GTK_GRID(grid), Abutton, 0, 1, 1, 1);
     gtk_widget_modify_bg(Abutton, GTK_STATE_NORMAL,&colorBlue);
     gtk_widget_modify_bg(Abutton, GTK_STATE_PRELIGHT,&colorWht);

     /* Create second button */
     GtkWidget *Bbutton = gtk_button_new_with_label("B");
     gtk_grid_attach(GTK_GRID(grid), Bbutton, 10, 1, 1, 1);
     gtk_widget_modify_bg(Bbutton, GTK_STATE_NORMAL,&colorRed2);
     gtk_widget_modify_bg(Bbutton, GTK_STATE_PRELIGHT,&colorWht);


     gtk_widget_show_all(window);
     gtk_main();

     return 0;
}
