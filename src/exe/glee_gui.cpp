#include <gui_glee/main_window.hpp>

void activate(GtkApplication* app, gpointer user_data)
{
        GtkWidget *main_window;

        main_window = mk_main_window(app, user_data);
        gtk_widget_show_all(main_window);
}

int main (int argc, char **argv)
{
        GtkApplication *app;
        int status;

        app = gtk_application_new("org.glee", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        status = g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);

        return status;
}
