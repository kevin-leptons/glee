#include <gui_glee/api.hpp>
#include <gui_glee/windows.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <iostream>

enum dir_view_col
{
        COL_DIR_ID = 0,
        COL_DIR_PATH,
        COL_DIR_LAST_SCAN,
        COL_DIR_SELECTED,
        NUM_COLS
};

enum dir_scan_view_col
{
        COL_DIR_SCAN_PATH = 0,
        COL_DIR_SCAN_STATE,
        DIR_SCAN_NUM_COLS
};

class media_dir_item {
public:
        size_t id;
        std::string path;

        media_dir_item() {}
};

static std::string _keyword = "";
static size_t _dir_page = 0;
static GtkWidget *_dir_view = NULL;
static GtkWidget *_container = NULL;
static GtkWidget *_dir_widget = NULL;
static GtkWidget *_dir_scan_widget = NULL;
static GtkWidget *_dir_scan_view = NULL;
static boost::thread _scan_thread;


static GtkListStore *_mk_dir_model(std::string keyword, size_t page)
{
        GtkListStore *model;
        GtkTreeIter iter;
        std::vector<glee::media_dir> items;
        std::string time_str;

        model = gtk_list_store_new(NUM_COLS,
                                G_TYPE_UINT64,     // identity of dir
                                G_TYPE_STRING,     // absolute path to dir
                                G_TYPE_STRING,     // last scan datetime
                                G_TYPE_BOOLEAN);   // row is selected or not
        items = glee_api->search_media_dir(keyword, page, FILE_ROWS_PP);
        for (auto it = items.begin(); it != items.end(); ++it) {
                time_str = time_to_str(&it->last_scan);
                gtk_list_store_append(model, &iter);
                 gtk_list_store_set(model, &iter,
                                   COL_DIR_ID, it->id,
                                   COL_DIR_PATH, it->path.c_str(),
                                   COL_DIR_LAST_SCAN, time_str.c_str(),
                                   COL_DIR_SELECTED, FALSE,
                                   -1);
        }

        return model;
}

static void _update_dir_view(void)
{
        GtkTreeModel *model;

        model = GTK_TREE_MODEL(_mk_dir_model(_keyword, _dir_page));
        gtk_tree_view_set_model(GTK_TREE_VIEW(_dir_view), model);
        g_object_unref(model);
}

static void _next_page_evt(void)
{
        _dir_page += 1;
        _update_dir_view();
}

static void _prev_page_evt(void)
{
        if (_dir_page == 0) return;
        _dir_page -= 1;
        _update_dir_view();
}

static void _search_evt(GtkSearchEntry *entry, gpointer user_data)
{
        _dir_page = 0;
        _keyword = gtk_entry_get_text(GTK_ENTRY(entry));
        _update_dir_view();
}

static std::vector<media_dir_item> _dir_selected_dirs(void)
{
        GtkTreeModel *model;
        GtkTreeIter iter;
        std::vector<media_dir_item> dir_items;

        model = gtk_tree_view_get_model(GTK_TREE_VIEW(_dir_view));
        if (!gtk_tree_model_get_iter_first(model, &iter))
                return dir_items;
        for (;;) {
                guint64 dir_id;
                gchar *dir_path;
                gboolean is_selected;
                gtk_tree_model_get(model, &iter,
                                   COL_DIR_ID, &dir_id,
                                   COL_DIR_PATH, &dir_path,
                                   COL_DIR_SELECTED, &is_selected,
                                   -1);
                if (is_selected) {
                        media_dir_item dir_item;
                        dir_item.id = dir_id;
                        dir_item.path = dir_path;
                        dir_items.push_back(dir_item);
                }
                g_free(dir_path);
                if (!gtk_tree_model_iter_next(model, &iter))
                        break;
        }

        return dir_items;
}

static GtkListStore *_mk_dir_scan_model(std::vector<media_dir_item> dir_items)
{
        GtkListStore *model;
        GtkTreeIter iter;

        model = gtk_list_store_new(DIR_SCAN_NUM_COLS,
                                G_TYPE_STRING,     // absolute path to dir
                                G_TYPE_STRING);    // state of scanning

        for (auto it = dir_items.begin(); it != dir_items.end(); ++it) {
                gtk_list_store_append(model, &iter);
                gtk_list_store_set(model, &iter,
                                   COL_DIR_SCAN_PATH, it->path.c_str(),
                                   COL_DIR_SCAN_STATE, "Prepend",
                                   -1);
        }

        return model;
}

static void _udpate_dir_scan_model(std::vector<media_dir_item> dirs)
{
        GtkListStore *model;

        model = _mk_dir_scan_model(dirs);
        gtk_tree_view_set_model(GTK_TREE_VIEW(_dir_scan_view),
                GTK_TREE_MODEL(model));
}

static void _scan_dir_thread_entry(void)
{
        std::vector<media_dir_item> dirs = _dir_selected_dirs();
        std::vector<size_t> dir_ids;

        _udpate_dir_scan_model(dirs);

        for (auto it = dirs.begin(); it != dirs.end(); ++it)
                dir_ids.push_back(it->id);
        glee_api->scan_by_dir_ids(dir_ids);

        gtk_container_remove(GTK_CONTAINER(_container), _dir_scan_widget);
        gtk_container_add(GTK_CONTAINER(_container), _dir_widget);
        // gtk_widget_show_all(_container);
}

static void _scan_dir_evt(void)
{
        gtk_container_remove(GTK_CONTAINER(_container), _dir_widget);
        gtk_container_add(GTK_CONTAINER(_container), _dir_scan_widget);
        gtk_widget_show_all(_container);

        _scan_thread = boost::thread(_scan_dir_thread_entry);
}

static void _cancel_scan_dir_evt(void)
{
        _scan_thread.interrupt();
        _scan_thread.join();

        gtk_container_remove(GTK_CONTAINER(_container), _dir_scan_widget);
        gtk_container_add(GTK_CONTAINER(_container), _dir_widget);
        gtk_widget_show_all(_container);
}

static void _remove_dir_evt(void)
{
        std::vector<media_dir_item> dirs = _dir_selected_dirs();

        for (auto it = dirs.begin(); it != dirs.end(); ++it)
                glee_api->del_media_dir(it->id);
        _update_dir_view();
}

static void _new_dir_evt(void)
{
        GtkWidget *dialog;
        gint ret;
        char *dir_path;

        dialog = gtk_file_chooser_dialog_new("Add directory to glee",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                      "Cancel", GTK_RESPONSE_CANCEL,
                                      "Select", GTK_RESPONSE_ACCEPT,
                                      NULL);
        ret = gtk_dialog_run(GTK_DIALOG(dialog));
        if (ret == GTK_RESPONSE_ACCEPT) {
              GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
              dir_path = gtk_file_chooser_get_filename(chooser);
              glee_api->add_media_dir(dir_path);
              g_free(dir_path);
        }
        gtk_widget_destroy(dialog);

        // update view
        _update_dir_view();
}

static GtkWidget *_mk_dir_scan_nav_bar(void)
{
        GtkWidget *nav_bar;
        GtkWidget *widget;

        nav_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

        // cancel scanning button
        widget = gtk_button_new_with_label("Cancel");
        gtk_box_pack_end(GTK_BOX(nav_bar), widget, FALSE, FALSE, 0);
        g_signal_connect(widget, "clicked",
                G_CALLBACK(_cancel_scan_dir_evt), NULL);

        return nav_bar;
}

static GtkWidget *_mk_dir_scan_view()
{
        GtkTreeViewColumn *col;
        GtkTreeSelection *sel;
        GtkWidget *view;
        GtkTreeModel *model;
        GtkCellRenderer *renderer;

        view = gtk_tree_view_new();
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), TRUE);
        gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), TRUE);
        sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
        gtk_tree_selection_set_mode(sel, GTK_SELECTION_NONE);

        // column: dir path
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, "Path");
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(col, renderer, TRUE);
        gtk_tree_view_column_add_attribute(col, renderer, "text",
                                           COL_DIR_SCAN_PATH);

        // column: state
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, "State");
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(col, renderer, TRUE);
        gtk_tree_view_column_add_attribute(col, renderer, "text",
                                      COL_DIR_SCAN_STATE);

        _dir_scan_view = view;
        return view;
}

static GtkWidget *_mk_dir_scan_widget(void)
{
        GtkWidget *vbox;
        GtkWidget *widget;
        GtkWidget *scroll;

        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

        widget = _mk_dir_scan_nav_bar();
        gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 0);

        scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
        widget = _mk_dir_scan_view();
        gtk_container_add(GTK_CONTAINER(scroll), widget);

        return vbox;
}

static GtkWidget *_mk_dir_nav_bar()
{
        GtkWidget *nav_bar;
        GtkWidget *widget;

        nav_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

        // prev button
        widget = gtk_button_new_with_label("Prev");
        gtk_box_pack_start(GTK_BOX(nav_bar), widget, FALSE, FALSE, 0);
        g_signal_connect(widget, "clicked", G_CALLBACK(_prev_page_evt), NULL);

        // search entry
        widget = gtk_search_entry_new();
        gtk_box_pack_start(GTK_BOX(nav_bar), widget, TRUE, TRUE, 8);
        g_signal_connect(widget, "search-changed",
                         G_CALLBACK(_search_evt), NULL);

        // next button
        widget = gtk_button_new_with_label("Next");
        gtk_box_pack_start(GTK_BOX(nav_bar), widget, FALSE, FALSE, 0);
        g_signal_connect(widget, "clicked", G_CALLBACK(_next_page_evt), NULL);

        // divide between search bar and action bar
        widget = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
        gtk_box_pack_start(GTK_BOX(nav_bar), widget, FALSE, FALSE, 8);

        // add dir button
        widget = gtk_button_new_with_label("Add");
        gtk_box_pack_start(GTK_BOX(nav_bar), widget, FALSE, FALSE, 0);
        g_signal_connect(widget, "clicked", G_CALLBACK(_new_dir_evt), NULL);

        // scan button
        widget = gtk_button_new_with_label("Scan");
        gtk_box_pack_start(GTK_BOX(nav_bar), widget, FALSE, FALSE, 8);
        g_signal_connect(widget, "clicked", G_CALLBACK(_scan_dir_evt), NULL);

        // remove button
        widget = gtk_button_new_with_label("Remove");
        gtk_box_pack_start(GTK_BOX(nav_bar), widget, FALSE, FALSE, 0);
        g_signal_connect(widget, "clicked", G_CALLBACK(_remove_dir_evt), NULL);

        return nav_bar;
}

static void _dir_row_toggle_evt(
        GtkCellRendererToggle *renderer,
        gchar *path,
        GtkTreeView *treeview
) {
        GtkTreeModel *model;
        GtkTreeIter iter;
        gboolean value;

        model = gtk_tree_view_get_model(treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path)) {
                gtk_tree_model_get(model, &iter,
                                   COL_DIR_SELECTED,
                                   &value, -1);
                gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                                   COL_DIR_SELECTED, !value, -1);
        }
}

static GtkWidget *_mk_dir_view()
{
        GtkTreeViewColumn *col;
        GtkTreeSelection *sel;
        GtkWidget *view;
        GtkTreeModel *model;
        GtkCellRenderer *renderer;

        view = gtk_tree_view_new();
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), TRUE);
        gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), TRUE);
        sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
        gtk_tree_selection_set_mode(sel, GTK_SELECTION_NONE);

        // column: dir id
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, "#");
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        gtk_tree_view_column_set_visible(col, FALSE);

        // column: dir path
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, "Path");
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(col, renderer, TRUE);
        gtk_tree_view_column_add_attribute(col, renderer, "text",
                                           COL_DIR_PATH);

        // column: last scan
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, "Last scan");
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(col, renderer, TRUE);
        gtk_tree_view_column_add_attribute(col, renderer, "text",
                                      COL_DIR_LAST_SCAN);

        // column: select to do action
        renderer = gtk_cell_renderer_toggle_new();
        g_object_set(renderer, "activatable", TRUE, NULL);
        col = gtk_tree_view_column_new_with_attributes(
                "",
                renderer,
                "active",
                COL_DIR_SELECTED,
                NULL
        );
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
        g_signal_connect(
                renderer, "toggled",
                G_CALLBACK(_dir_row_toggle_evt), view
        );

        _dir_view = view;
        return view;
}

static GtkWidget *_mk_dir_widget(void)
{
        GtkWidget *vbox;
        GtkWidget *scroll;
        GtkWidget *widget;

        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

        widget = _mk_dir_nav_bar();
        gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 0);

        scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
        widget = _mk_dir_view();
        gtk_container_add(GTK_CONTAINER(scroll), widget);
        _update_dir_view();

        return vbox;
}

GtkWidget *mk_setting_tab(void)
{
        _container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

        _dir_scan_widget = _mk_dir_scan_widget();
        g_object_ref(_dir_scan_widget);

        _dir_widget = _mk_dir_widget();
        g_object_ref(_dir_widget);
        gtk_box_set_homogeneous(GTK_BOX(_container), true);
        gtk_container_add(GTK_CONTAINER(_container), _dir_widget);

        return _container;
}
