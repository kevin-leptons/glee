#include <glee/media_file_table.hpp>

namespace glee {

void media_file_table::add(size_t dir_id, 
                const boost::filesystem::path &file_path)
{
        int ret;
        std::string dir_path = file_path.parent_path().string();
        std::string file_name = file_path.filename().string();
        sqlite3_stmt *stmt;
        char sql[] = "INSERT INTO media_file " \
                      "(dir_id, dir_path, file_name) VALUES (?, ?, ?)";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) goto finish;

        ret = sqlite3_bind_int64(stmt, 1, dir_id);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_text(stmt, 2, dir_path.c_str(), 
                                dir_path.length() + 1, NULL);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_text(stmt, 3, file_name.c_str(),
                                file_name.length() + 1, NULL);
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

std::vector<media_file> media_file_table::search(std::string keyword,
                                              size_t page, size_t rows_pp)
{
        int ret;
        sqlite3_stmt *stmt;
        std::vector<media_file> rows;
        std::string search_kw = "%" + keyword + "%";
        char sql[] = "SELECT id, dir_id, dir_path, file_name " \
                     "FROM media_file " \
                     "WHERE dir_path LIKE ? OR file_name LIKE ? " \
                     "LIMIT ? OFFSET ? COLLATE NOCASE";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) goto finish;

        ret = sqlite3_bind_text(stmt, 1, search_kw.c_str(), 
                                search_kw.length() + 1, NULL);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_text(stmt, 2, search_kw.c_str(), 
                                search_kw.length() + 1, NULL);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_int64(stmt, 3, rows_pp);
        if (ret != SQLITE_OK) goto free_stmt;
        ret = sqlite3_bind_int64(stmt, 4, page * rows_pp);
        if (ret != SQLITE_OK) goto free_stmt;

        for (;;) {
                ret = sqlite3_step(stmt);
                if (ret == SQLITE_DONE) break;
                if (ret != SQLITE_ROW) goto free_stmt;
                media_file row;
                row.id = sqlite3_column_int64(stmt, 0);
                row.dir_id = sqlite3_column_int64(stmt, 1);
                row.dir_path = (char *) sqlite3_column_text(stmt, 2);
                row.file_name = (char * ) sqlite3_column_text(stmt, 3);
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

void media_file_table::_create()
{
        int ret;
        sqlite3_stmt *stmt;
        char sql[] = "CREATE TABLE IF NOT EXISTS media_file (" \
                     "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "dir_id     INTEGER                NOT NULL, " \
                     "dir_path   TEXT                   NOT NULL," \
                     "file_name  TEXT                   NOT NULL)";

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

media_file media_file_table::find_by_id(size_t identity)
{
        int ret;
        sqlite3_stmt *stmt;
        media_file row;
        char sql[] = "SELECT id, dir_path, file_name " \
                     "FROM media_file WHERE id = ?";

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
                row.dir_path = (char *) sqlite3_column_text(stmt, 1);
                row.file_name = (char * ) sqlite3_column_text(stmt, 2);
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

void media_file_table::del_by_dir_id(size_t dir_id)
{
        int ret;
        sqlite3_stmt *stmt;
        const char *sql = "DELETE FROM media_file "\
                          "WHERE dir_id = ?";

        ret = sqlite3_prepare_v2(_db.sqlite_handle(), sql, strlen(sql) + 1,
                                 &stmt, NULL);
        if (ret != SQLITE_OK) goto finish;
        ret = sqlite3_bind_int64(stmt, 1, dir_id);
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

}
