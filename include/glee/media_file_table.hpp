#ifndef GLEE_MEDIA_FILE_TABLE_HPP
#define GLEE_MEDIA_FILE_TABLE_HPP

#include <glee/database.hpp>

namespace glee {

class media_file {
public:
        size_t id;
        size_t dir_id;
        boost::filesystem::path dir_path;
        std::string file_name;

        media_file() {}
};

class media_file_table {
public:
        media_file_table() {}
        media_file_table(const glee::database &db):
                _db(db)
        {
                _create();
        }
        ~media_file_table() {}

        void add(size_t dir_id, const boost::filesystem::path &file_path);
        std::vector<media_file> search(std::string keyword, 
                                    size_t page, size_t rows_pp);
        media_file find_by_id(size_t identity);
        void del_by_dir_id(size_t dir_id);

protected:
        glee::database _db;

        void _create();
};

}

#endif
