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

class index_error : public std::runtime_error {
public:
        index_error(std::string message):
                std::runtime_error(message)
        {}
};

class api_config {
public: 
        boost::filesystem::path db_file_path;

        api_config(boost::filesystem::path db_file_path):
                db_file_path(db_file_path)
        {}
};

class api {
protected:
        glee::api_config _config;

public:
        api(const glee::api_config &config):
                _config(config),
                _db(config.db_file_path),
                _media_file_table(_db),
                _media_dir_table(_db)
        {
        }
        ~api(void) {}

/*
        Remove all of media files in specific directory then re-scan that
        directory.
*/
        void scan_by_dir_id(size_t dir_id);

/*
        Retrieve media files in database
*/
        std::vector<glee::media_file> search_media_file(std::string keyword,
                                                  size_t page, size_t rows_pp)
        {
                return _media_file_table.search(keyword, page, rows_pp);
        }

/*
        Play media file by default application.
*/
        void play_media_file(size_t identity);

/*
        Find media file match with keyword, such as theme or type, etc
        and select order by iterator.
*/
        glee::media_file master_find_media_file(std::string keyword,
                                                size_t iterator)
        {
                std::vector<media_file> files;
                files = _media_file_table.search(keyword, iterator, 1);
                if (files.size() == 0)
                        throw glee::index_error("Index is invalid");
                else
                        return files[0];
        }

        glee::media_file find_media_file_by_id(size_t id)
        {
                return _media_file_table.find_by_id(id);
        }

        void add_media_dir(boost::filesystem::path dir_path)
        {
                _media_dir_table.add(dir_path);
        }

        glee::media_dir  find_media_dir_by_id(size_t id)
        {
                return _media_dir_table.find_by_id(id);
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
