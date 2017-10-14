#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <boost/filesystem.hpp>
#include <boost/process/spawn.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/io.hpp>
#include <ctime>

void file_scan(const char *dir)
{
        boost::filesystem::path path;
        boost::filesystem::recursive_directory_iterator it;
        boost::filesystem::recursive_directory_iterator end;
        std::string fpath;
        std::string file_ext;
        std::unordered_set<std::string> ext_set;
        std::time_t last_write;

        path = "/mnt/resources/movies";
        ext_set.insert(".mp4");
        ext_set.insert(".mkv");
        ext_set.insert(".mp3");

        if (!boost::filesystem::is_directory(path))
                return;

        it = boost::filesystem::recursive_directory_iterator(path);
        for (end; it != end; ++it) {
                fpath = it->path().string();
                file_ext = it->path().extension().string();
                if (ext_set.find(file_ext) != ext_set.end()) {
                        last_write = boost::filesystem::last_write_time(fpath);
                        std::cout << fpath << "===" << last_write << std::endl;
                }
        }

}

void open_vlc(const char *path)
{
        boost::process::spawn(boost::process::search_path("vlc"),  path,
                              boost::process::std_out > boost::process::null,
                              boost::process::std_err > boost::process::null,
                              boost::process::std_in < boost::process::null);
}
