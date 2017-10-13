#ifndef GLEE_MEDIA_DIR_TABLE_HPP
#define GLEE_MEDIA_DIR_TABLE_HPP

#include <glee/database.hpp>

namespace glee {

class media_dir {
public:
        size_t id;
        boost::filesystem::path path;
        std::time_t last_scan;

        media_dir() {}
};

class media_dir_table {
public:
        media_dir_table();
        media_dir_table(const glee::database &db):
                _db(db)
        {
                _create();
        }

        void add(const boost::filesystem::path &dir_path);
        std::vector<media_dir> search(std::string keyword, 
                                      size_t page, size_t rows_pp);
        media_dir find_by_id(size_t identity);
        void del(size_t identity);

protected:
        glee::database _db;

        void _create();
};

}

#endif
