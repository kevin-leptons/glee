#include <gui_glee/api.hpp>
#include <gui_glee/main_window.hpp>
#include <gui_glee/windows.hpp>

GtkWidget *mk_main_window(GtkApplication* app, gpointer user_data)
{
        GtkWidget *window;
        GtkWidget *widget;
        GtkWidget *switcher;
        GtkWidget *tab;
        GtkWidget *vbox;
        GtkWidget *book;
        GtkWidget *page_label;

        init_glee_api();

        window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(window), "Glee");
        gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

        book = gtk_notebook_new();
        gtk_container_add(GTK_CONTAINER(window), book);

        widget = mk_file_explorer_tab();
        page_label = gtk_label_new("Files");
        gtk_notebook_append_page(GTK_NOTEBOOK(book), widget, page_label);

        widget = mk_setting_tab();
        page_label = gtk_label_new("Setting");
        gtk_notebook_append_page(GTK_NOTEBOOK(book), widget, page_label);

        return window;
}
