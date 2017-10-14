#include <glee_cmd/api.hpp>

#include <iostream>
#include <glee/api.hpp>

namespace glee { namespace cmd {

char const *PAGE_NUMBER_MUST_BE_GT_THAN_ZERO =
        "Page must be greater than zero";

boost::filesystem::path _home_dir(void)
{
        char const *path; 
        char const *hard_drive;
        
        path = getenv("HOME");
        if (path) return path;
        
        path = getenv("USERPROFILE");
        if (path) return path;

        hard_drive = getenv("HOMEDRIVE");
        if (!hard_drive) goto error;
        path = getenv("HOMEPATH");
        if (!path) goto error;
        return boost::filesystem::path(hard_drive) /= path;

error:
        throw std::runtime_error("Can not detect home directory");
}

glee::api api(void)
{
        boost::filesystem::path home_dir = _home_dir();
        home_dir /= ".glee.db";
        glee::api_config config(home_dir);

        return glee::api(config);
}

void exit_error(const std::string &message)
{
        std::cerr << "Error: " << message << std::endl;
        exit(EXIT_FAILURE);
}

std::string limit_str_size(const std::string &str, size_t limit_size)
{
        if (str.size() < limit_size)
                return std::string(str);
        else
                return str.substr(0, limit_size) + "...";
}

} }
