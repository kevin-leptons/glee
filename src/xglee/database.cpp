#include <glee/database.hpp>

namespace glee {

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

sqlite3 *database::sqlite_handle()
{
        return _sqlite_handle;
}

}
