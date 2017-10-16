#include <glee/database.hpp>

#include <iostream>

namespace glee {

statement::statement(sqlite3 *db_handle, std::string sql) :
        _sqlite(db_handle)
{
        int ret;

        ret = sqlite3_prepare_v2(_sqlite, sql.c_str(),
                                 sql.size() + 1, &_stmt, NULL);
        if (ret != SQLITE_OK) {
                sqlite3_finalize(_stmt);
                throw sqlite_error(_sqlite);
        }
}

statement::~statement()
{
        sqlite3_finalize(_stmt);
}

void statement::bind_int(int param_index, int value)
{
        int ret;

        ret = sqlite3_bind_int(_stmt, param_index, value);
        if (ret != SQLITE_OK)
                throw sqlite_error(_sqlite);
}

void statement::bind_int64(int param_index, uint64_t value)
{
        int ret;

        ret = sqlite3_bind_int64(_stmt, param_index, value);
        if (ret != SQLITE_OK)
                throw sqlite_error(_sqlite);
}

void statement::bind_text(int param_index, const std::string &value)
{
        int ret;
        ret = sqlite3_bind_text(_stmt, param_index,
                                value.c_str(), value.size() + 1, NULL);
        if (ret != SQLITE_OK)
                throw sqlite_error(_sqlite);
}

int statement::step(void)
{
        return sqlite3_step(_stmt);
}

int statement::column_int(int col_index)
{
        return sqlite3_column_int(_stmt, col_index);
}

uint64_t statement::column_int64(int col_index)
{
        return sqlite3_column_int64(_stmt, col_index);
}

std::string statement::column_text(int col_index)
{
        std::string value = (char *) sqlite3_column_text(_stmt, col_index);
        return value;
}

database::database(const boost::filesystem::path &db_file)
{
        if (sqlite3_open(db_file.c_str(), &_sqlite_handle) != SQLITE_OK)
                throw sqlite_error(sqlite3_errmsg(_sqlite_handle));
}

database::~database()
{
        if (_sqlite_handle)
                sqlite3_close(_sqlite_handle);
}

statement database::mk_statement(std::string sql)
{
        return statement(_sqlite_handle, sql);
}

sqlite3 *database::sqlite_handle()
{
        return _sqlite_handle;
}

}
