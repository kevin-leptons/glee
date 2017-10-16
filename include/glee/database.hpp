/*
NAME
====

        database - simple SQL database.

DESCRIPTION
===========

        This module port part of sqlite3 APIs in C to C++ for using in glee.
*/

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
        
        sqlite_error(sqlite3 *handle) :
                sqlite_error(sqlite3_errmsg(handle))
        {}
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

class statement {
public:
        statement(sqlite3 *db_handle, std::string sql);
        ~statement();

        void bind_int(int param_index, int value);
        void bind_int64(int param_index, uint64_t value);
        void bind_text(int param_index, const std::string &value);

        int step(void);
        int column_int(int col_index);
        uint64_t column_int64(int col_index);
        std::string column_text(int col_index);

protected:
        sqlite3 *_sqlite;
        sqlite3_stmt *_stmt;
};

class database {
public:
        database() {}
        database(const boost::filesystem::path &db_file);
        ~database();

        statement mk_statement(std::string sql);
        sqlite3 *sqlite_handle();

protected:
        sqlite3 *_sqlite_handle = NULL;
};

}

#endif
