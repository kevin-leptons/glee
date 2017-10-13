#include <gui_glee/api.hpp>
#include <gui_glee/windows.hpp>

static std::string _keyword = "";
static size_t _file_page = 0;
static GtkWidget *_file_view;

enum file_view_col
{
        COL_FILE_ID = 0,
        COL_FILE_NAME,
        NUM_COLS
};

static GtkTreeModel *_create_file_model(std::string keyword, size_t page)
{
        GtkTreeStore *model;
        GtkTreeIter iter;
        std::vector<glee::media_file> items;

        model = gtk_tree_store_new(NUM_COLS,
                                G_TYPE_UINT64,  // identity of file
                                G_TYPE_STRING); // file name
        items = glee_api->search_media_file(keyword, page, FILE_ROWS_PP);
        for (auto it = items.begin(); it != items.end(); ++it) {
                gtk_tree_store_append(model, &iter, NULL);
                gtk_tree_store_set(model, &iter,
                                   COL_FILE_ID, it->id,
                                   COL_FILE_NAME, it->file_name.c_str(),
                                   -1);
        }
        return GTK_TREE_MODEL(model);
}

static void _double_click_on_list(GtkTreeView        *treeview,
                         GtkTreePath        *path,
                         GtkTreeViewColumn  *col,
                         gpointer            userdata)
{
        GtkTreeModel *model;
        GtkTreeIter   iter;

        model = gtk_tree_view_get_model(treeview);

        if (gtk_tree_model_get_iter(model, &iter, path))
        {
                guint64 id;

                gtk_tree_model_get(model, &iter, COL_FILE_ID, &id, -1);
                glee_api->show_media_file(id);
        }
}

static GtkWidget *_mk_file_view()
{
        GtkTreeViewColumn *col;
        GtkWidget *view;
        GtkTreeModel *model;
        GtkCellRenderer *renderer;

        view = gtk_tree_view_new();
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);
        g_signal_connect(view, "row-activated",
                        (GCallback) _double_click_on_list, NULL);

        // file id
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, "#");
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        gtk_tree_view_column_set_visible(col, FALSE);

        // file name column
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, "Name");
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(col, renderer, TRUE);
        gtk_tree_view_column_add_attribute(col, renderer, "text",
                                           COL_FILE_NAME);

        // data
        model = _create_file_model(_keyword, _file_page);
        gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
        g_object_unref(model);

        _file_view = view;
        return view;
}

static GtkWidget *_mk_list_panel()
{
        GtkWidget *scrool_view;
        GtkWidget *file_list;

        file_list = _mk_file_view();
        scrool_view = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(scrool_view), file_list);
        return scrool_view;
}

static void _next_page()
{
        GtkTreeModel *model;

        _file_page += 1;
        model = _create_file_model(_keyword, _file_page);
        gtk_tree_view_set_model(GTK_TREE_VIEW(_file_view), model);
        g_object_unref(model);
}

static void _prev_page()
{
        GtkTreeModel *model;

        if (_file_page == 0)
                return;
        _file_page -= 1;
        model = _create_file_model(_keyword, _file_page);
        gtk_tree_view_set_model(GTK_TREE_VIEW(_file_view), model);
        g_object_unref(model);
}

static void _search_change(GtkSearchEntry *entry, gpointer user_data)
{
        GtkTreeModel *model;

        _file_page = 0;
        _keyword = gtk_entry_get_text(GTK_ENTRY(entry));

        model = _create_file_model(_keyword, _file_page);
        gtk_tree_view_set_model(GTK_TREE_VIEW(_file_view), model);
        g_object_unref(model);
}

static GtkWidget *_mk_nav_bar()
{
        GtkWidget *button;
        GtkWidget *nav_bar;
        GtkWidget *search;

        nav_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

        // prev button
        button = gtk_button_new_with_label("Prev");
        gtk_box_pack_start(GTK_BOX(nav_bar), button, FALSE, FALSE, 16);
        g_signal_connect(button, "clicked", G_CALLBACK(_prev_page), NULL);

        // search entry
        search = gtk_search_entry_new();
        gtk_box_pack_start(GTK_BOX(nav_bar), search, TRUE, TRUE, 0);
        g_signal_connect(search, "search-changed",
                         G_CALLBACK(_search_change), NULL);

        // next button
        button = gtk_button_new_with_label("Next");
        gtk_box_pack_start(GTK_BOX(nav_bar), button, FALSE, FALSE, 16);
        g_signal_connect(button, "clicked", G_CALLBACK(_next_page), NULL);

        return nav_bar;
}

GtkWidget *mk_file_explorer_tab(void)
{

        GtkWidget *vbox;
        GtkWidget *file_list;
        GtkWidget *child_window;

        // main layout
        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

        // button bar
        child_window = _mk_nav_bar();
        gtk_box_pack_start(GTK_BOX(vbox), child_window, FALSE, FALSE, 16);

        // file list view
        child_window = _mk_list_panel();
        gtk_box_pack_start(GTK_BOX(vbox), child_window, TRUE, TRUE, 0);

        return vbox;
}
