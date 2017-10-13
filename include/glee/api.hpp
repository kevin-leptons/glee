/*
NAME
====

        api - top of glee API.

DESCRIPTION
===========

        Export higest level of glee to work with multi-media files.
*/

#ifndef GLEE_API_HPP
#define GLEE_API_HPP

#include <vector>
#include <boost/filesystem.hpp>
#include <glee/database.hpp>
#include <glee/media_file_table.hpp>
#include <glee/media_dir_table.hpp>

namespace glee {

class api_config {
public: 
        std::vector<boost::filesystem::path> media_dirs;
        std::string db_file_path;
};

class api {
protected:
        glee::api_config _config;

public:
        api() {}
        api(const glee::api_config &config):
                _config(config),
                _db(config.db_file_path),
                _media_file_table(_db),
                _media_dir_table(_db)
        {
        }
        ~api(void) {}

/*
        Scan configuration directories and create index for all of multi-media
        file.
 */
        void scan_filesystem(void);

/*
        Remove all of media files in specific directory then re-scan this
        directory.
*/
        void scan_by_dir_ids(std::vector<size_t> dir_ids);

/*
        Retrieve media files in database
*/
        std::vector<glee::media_file> search_media_file(std::string keyword,
                                                  size_t page, size_t rows_pp)
        {
                return _media_file_table.search(keyword, page, rows_pp);
        }

/*
        Show media file by default application.
*/
        void show_media_file(size_t identity);

        void add_media_dir(boost::filesystem::path dir_path)
        {
                _media_dir_table.add(dir_path);
        }

        const std::vector<glee::media_dir> search_media_dir(
                        std::string keyword,
                        size_t page, size_t rows_pp)
        {
                return _media_dir_table.search(keyword, page, rows_pp);
        }

        void del_media_dir(size_t identity)
        {
                _media_file_table.del_by_dir_id(identity);
                _media_dir_table.del(identity);
        }

protected:

        glee::database _db;
        glee::media_file_table _media_file_table;
        glee::media_dir_table _media_dir_table;

        void _scan_directory(size_t dir_id, 
                        const boost::filesystem::path &dir_path);
};

}

#endif
