#include <glee/media_dir_table.hpp>

namespace glee {

void media_dir_table::add(const boost::filesystem::path &dir_path)
{
        int ret;
        glee::statement stmt = _db.mk_statement(
                "INSERT INTO media_dir " \
                "(path, last_scan) VALUES (?, ?)"
        );
        stmt.bind_text(1, dir_path.string());
        stmt.bind_int(2, std::time(nullptr));

        ret = stmt.step();
        if (ret != SQLITE_DONE)
                throw sqlite_error(_db.sqlite_handle());
}

std::vector<media_dir> media_dir_table::search(std::string keyword, 
                                                size_t page, size_t rows_pp)
{
        int ret;
        std::vector<media_dir> rows;
        std::string search_kw = "%" + keyword + "%";
        statement stmt = _db.mk_statement(
                "SELECT id, path, last_scan " \
                "FROM media_dir " \
                "WHERE path LIKE ? " \
                "LIMIT ? OFFSET ? COLLATE NOCASE"
        );
        stmt.bind_text(1, search_kw);
        stmt.bind_int64(2, rows_pp);
        stmt.bind_int64(3, page * rows_pp);

        for (;;) {
                ret = stmt.step();
                if (ret == SQLITE_DONE) break;
                if (ret != SQLITE_ROW) 
                        throw sqlite_error(_db.sqlite_handle());
                media_dir row;
                row.id = stmt.column_int64(0);
                row.path = stmt.column_text(1);
                row.last_scan = stmt.column_int(2);
                rows.push_back(row);
        }
        return rows;
}

media_dir media_dir_table::find_by_id(size_t identity)
{
        int ret;
        statement stmt = _db.mk_statement(
                "SELECT id, path, last_scan " \
                "FROM media_dir WHERE id = ?"
        );
        stmt.bind_int64(1, identity);

        ret = stmt.step();
        if (ret == SQLITE_ROW) {
                media_dir row;
                row.id = stmt.column_int64(0);
                row.path = stmt.column_text(1);
                row.last_scan = stmt.column_int(2);
                return row;

        } else if (ret == SQLITE_DONE) {
                throw sqlite_key_error(identity);
        } else {
                throw sqlite_error(_db.sqlite_handle());
        }
}

void media_dir_table::del(size_t identity)
{
        int ret;
        statement stmt = _db.mk_statement(
                "DELETE FROM media_dir " \
                "WHERE id = ?"
        );
        stmt.bind_int64(1, identity);

        ret = stmt.step();
        if (ret != SQLITE_DONE)
                throw sqlite_error(_db.sqlite_handle());
}

void media_dir_table::_create()
{
        int ret;
        statement stmt = _db.mk_statement(
                "CREATE TABLE IF NOT EXISTS media_dir (" \
                "id         INTEGER PRIMARY KEY AUTOINCREMENT," \
                "path       TEXT                   NOT NULL," \
                "last_scan  INTEGER                NOT NULL)"
        );

        ret = stmt.step();
        if (ret != SQLITE_DONE)
                throw sqlite_error(_db.sqlite_handle());
}

}
