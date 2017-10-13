#include <gui_glee/api.hpp>

#define TIME_CSTR_LEN 19

glee::api *glee_api;

void init_glee_api(void)
{
        glee::api *api;
        glee::api_config config;
        config.media_dirs.push_back("/mnt/resources/movies");
        config.db_file_path = "tmp/glee.db";

        glee_api = new glee::api(config);
}

std::string time_to_str(const std::time_t *t)
{
        std::tm *tm;
        char time_cstr[TIME_CSTR_LEN + 1];

        tm = std::localtime(t);
        strftime(time_cstr, TIME_CSTR_LEN + 1, "%H:%M:%S %Y-%m-%d", tm);

        return std::string(time_cstr);
}
