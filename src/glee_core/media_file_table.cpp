#include <glee/media_file_table.hpp>

namespace glee {

void media_file_table::add(size_t dir_id, 
                const boost::filesystem::path &file_path)
{
        int ret;
        statement stmt = _db.mk_statement(
                "INSERT INTO media_file " \
                "(dir_id, dir_path, file_name) VALUES (?, ?, ?)"
        );

        std::string dir_path = file_path.parent_path().string();
        std::string file_name = file_path.filename().string();
        stmt.bind_int64(1, dir_id);
        stmt.bind_text(2, dir_path);
        stmt.bind_text(3, file_name);

        ret =stmt.step();
        if (ret != SQLITE_DONE) 
                throw sqlite_error(_db.sqlite_handle());
}

std::vector<media_file> media_file_table::search(std::string keyword,
                                              size_t page, size_t rows_pp)
{
        int ret;
        std::vector<media_file> rows;
        std::string search_kw = "%" + keyword + "%";
        statement stmt = _db.mk_statement(
                "SELECT id, dir_id, dir_path, file_name " \
                "FROM media_file " \
                "WHERE dir_path LIKE ? OR file_name LIKE ? " \
                "LIMIT ? OFFSET ? COLLATE NOCASE"
        );
        stmt.bind_text(1, search_kw);
        stmt.bind_text(2, search_kw);
        stmt.bind_int64(3, rows_pp);
        stmt.bind_int64(4, page * rows_pp);

        for (;;) {
                ret = stmt.step();
                if (ret == SQLITE_DONE) break;
                if (ret != SQLITE_ROW)
                        throw sqlite_error(_db.sqlite_handle());
                media_file row;
                row.id = stmt.column_int64(0);
                row.dir_id = stmt.column_int64(1);
                row.dir_path = stmt.column_text(2);
                row.file_name = stmt.column_text(3);
                rows.push_back(row);
        }

        return rows;
}

void media_file_table::_create()
{
        int ret;
        statement stmt = _db.mk_statement(
                "CREATE TABLE IF NOT EXISTS media_file (" \
                "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                "dir_id     INTEGER                NOT NULL, " \
                "dir_path   TEXT                   NOT NULL," \
                "file_name  TEXT                   NOT NULL)"
        );

        if (stmt.step() != SQLITE_DONE)
                throw sqlite_error(_db.sqlite_handle());
}

media_file media_file_table::find_by_id(size_t identity)
{
        int ret;
        media_file row;
        statement stmt = _db.mk_statement(
                "SELECT id, dir_path, file_name " \
                "FROM media_file WHERE id = ?"
        );
        stmt.bind_int64(1, identity);

        ret = stmt.step();
        if (ret == SQLITE_ROW) {
                row.id = stmt.column_int64(0);
                row.dir_path = stmt.column_text(1);
                row.file_name = stmt.column_text(2);
                return row;

        } else if (ret == SQLITE_DONE) {
                throw sqlite_key_error(identity);
        } else {
                throw sqlite_error(_db.sqlite_handle());
        }
}

void media_file_table::del_by_dir_id(size_t dir_id)
{
        int ret;
        statement stmt = _db.mk_statement(
                "DELETE FROM media_file " \
                "WHERE dir_id = ?"
        );
        stmt.bind_int64(1, dir_id);

        if (stmt.step() != SQLITE_DONE)
                throw sqlite_error(_db.sqlite_handle());
}

}
