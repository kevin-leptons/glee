#include <glee/media_dir_table.hpp>

namespace glee {

void media_dir_table::add(const boost::filesystem::path &dir_path)
{
        int ret;
        sqlite3_stmt *stmt;
        char sql[] = "INSERT INTO media_dir " \
                      "(path, last_scan) VALUES (?, ?)";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) goto finish;

        ret = sqlite3_bind_text(stmt, 1, dir_path.string().c_str(), 
                                dir_path.string().length() + 1, NULL);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_int(stmt, 2, std::time(nullptr));
        if (ret != SQLITE_OK) goto free_stmt;

        ret = sqlite3_step(stmt);
        if (ret != SQLITE_DONE) goto free_stmt;
        ret = 0;

free_stmt:
        sqlite3_finalize(stmt);
finish:
        if (ret)
                throw sqlite_error(sqlite3_errmsg(_db.sqlite_handle()));
}

std::vector<media_dir> media_dir_table::search(std::string keyword, 
                                                size_t page, size_t rows_pp)
{
        int ret;
        sqlite3_stmt *stmt;
        std::vector<media_dir> rows;
        std::string search_kw = "%" + keyword + "%";
        char sql[] = "SELECT id, path, last_scan " \
                     "FROM media_dir " \
                     "WHERE path LIKE ? " \
                     "LIMIT ? OFFSET ? COLLATE NOCASE";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) goto finish;

        ret = sqlite3_bind_text(stmt, 1, search_kw.c_str(), 
                                search_kw.length() + 1, NULL);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_int64(stmt, 2, rows_pp);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_int64(stmt, 3, page * rows_pp);
        if (ret != SQLITE_OK) goto free_stmt;

        for (;;) {
                ret = sqlite3_step(stmt);
                if (ret == SQLITE_DONE) break;
                if (ret != SQLITE_ROW) goto free_stmt;
                media_dir row;
                row.id = sqlite3_column_int64(stmt, 0);
                row.path = (char *) sqlite3_column_text(stmt, 1);
                row.last_scan = sqlite3_column_int(stmt, 2);
                rows.push_back(row);
        }

        ret = 0;

free_stmt:
        sqlite3_finalize(stmt);
finish:
        if (ret)
                throw sqlite_error(sqlite3_errmsg(_db.sqlite_handle()));
        return rows;
}

media_dir media_dir_table::find_by_id(size_t identity)
{
        int ret;
        sqlite3_stmt *stmt;
        media_dir row;
        char sql[] = "SELECT id, path, last_scan " \
                     "FROM media_dir WHERE id = ?";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) 
                throw sqlite_error(sqlite3_errmsg(_db.sqlite_handle()));

        ret = sqlite3_bind_int64(stmt, 1, identity);
        if (ret != SQLITE_OK)
                throw sqlite_error(sqlite3_errmsg(_db.sqlite_handle()));

        ret = sqlite3_step(stmt);
        if (ret == SQLITE_ROW) {
                row.id = sqlite3_column_int64(stmt, 0);
                row.path = (char *) sqlite3_column_text(stmt, 1);
                row.last_scan = sqlite3_column_int(stmt, 2);
                sqlite3_finalize(stmt);
                return row;

        } else if (ret == SQLITE_DONE) {
                sqlite3_finalize(stmt);
                throw sqlite_key_error(identity);
        } else {
                sqlite3_finalize(stmt);
                throw sqlite_error(sqlite3_errmsg(_db.sqlite_handle()));
        }
}

void media_dir_table::del(size_t identity)
{
        int ret;
        sqlite3_stmt *stmt;
        char sql[] = "DELETE FROM media_dir " \
                     "WHERE id = ?";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) goto finish;
        ret = sqlite3_bind_int64(stmt, 1, identity);
        if (ret != SQLITE_OK) goto finish;

        ret = sqlite3_step(stmt);
        if (ret != SQLITE_DONE) goto finish;

        ret = 0;

free_stmt:
        sqlite3_finalize(stmt);
finish:
        if (ret)
                throw sqlite_error(sqlite3_errmsg(_db.sqlite_handle()));
}

void media_dir_table::_create()
{
        int ret;
        sqlite3_stmt *stmt;
        char sql[] = "CREATE TABLE IF NOT EXISTS media_dir (" \
                     "id         INTEGER PRIMARY KEY AUTOINCREMENT," \
                     "path       TEXT                   NOT NULL," \
                     "last_scan  INTEGER                NOT NULL)";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) goto finish;

        ret = sqlite3_step(stmt);
        if (ret != SQLITE_DONE) goto free_stmt;

        ret = 0;

free_stmt:
        sqlite3_finalize(stmt);
finish:
        if (ret)
                throw sqlite_error(sqlite3_errmsg(_db.sqlite_handle()));
}

}
