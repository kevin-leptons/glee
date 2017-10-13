#ifndef GLEE_DATABASE_HPP
#define GLEE_DATABASE_HPP

#include <stdexcept>
#include <string>
#include <boost/filesystem.hpp>
#include <sqlite3.h>

namespace glee {

class sqlite_error : public std::runtime_error {
public:
        sqlite_error(const char *message) : std::runtime_error(message)
        {
        }

};

class sqlite_key_error : public std::runtime_error {
public:
        sqlite_key_error(size_t key) : std::runtime_error(std::to_string(key))
        {
        }

        sqlite_key_error(const char * message) : std::runtime_error(message)
        {
        }
};

class database {
public:
        database() {}
        database(const boost::filesystem::path &db_file);
        ~database();

        sqlite3 *sqlite_handle();

protected:
        sqlite3 *_sqlite_handle = NULL;
};

}

#endif
