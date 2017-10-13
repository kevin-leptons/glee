#ifndef GLEE_MEDIA_FILE_ITERATOR_HPP
#define GLEE_MEDIA_FILE_ITERATOR_HPP

#include <memory>
#include <boost/filesystem.hpp>

typedef boost::filesystem::path boost_fs_path;
typedef boost::filesystem::recursive_directory_iterator recursive_dir_it;

namespace glee {

class media_file_iterator {
public:
        class iterator {
        public:

                iterator(const recursive_dir_it &dir_it) { _dir_it = dir_it; }
                iterator(const iterator &it) { _dir_it = it._dir_it; }
                ~iterator() {}

                iterator& operator=(const iterator &it) 
                { 
                        _dir_it = it._dir_it;
                }
                bool operator==(const iterator &it) const
                {
                        return _dir_it == it._dir_it;
                }
                bool operator!=(const iterator &it) const
                {
                        return _dir_it != it._dir_it;
                }

                iterator& operator++() { _dir_it++; }

                const boost_fs_path& operator*() { return _dir_it->path(); }
                const boost_fs_path operator->() { return _dir_it->path(); }

                boost_fs_path path() { return _dir_it->path(); }
        private:
                recursive_dir_it _dir_it;
        };

        class const_iterator {
        public:
                const_iterator();
                const_iterator(const const_iterator&);
                const_iterator(const iterator&);
                ~const_iterator();

                const_iterator& operator=(const const_iterator&);
                bool operator==(const const_iterator&) const;
                bool operator!=(const const_iterator&) const;

                const_iterator& operator++();

                boost_fs_path operator*() const;
                boost_fs_path operator->() const;
        };

        media_file_iterator(const boost_fs_path &dir_path)
        {
                _dir_path = dir_path;
        }
        media_file_iterator(const media_file_iterator &it)
        {
                _dir_it = it._dir_it;
        }
        ~media_file_iterator() {}


        //media_file_iterator& operator=(const media_file_iterator &it)
        //{
                //return media_file_iterator(it);
        //}
        bool operator==(const media_file_iterator &it) const
        {
                return _dir_it == it._dir_it;
        }
        bool operator!=(const media_file_iterator &it) const
        {
                return _dir_it != it._dir_it;
        }

        iterator begin() { return iterator(recursive_dir_it(_dir_path)); }
        const_iterator begin() const;
        const_iterator cbegin() const;
        iterator end() { return iterator(recursive_dir_it()); }
        const_iterator end() const;
        const_iterator cend() const;
private:
        boost_fs_path _dir_path;
        recursive_dir_it _dir_it;
};

}

#endif
