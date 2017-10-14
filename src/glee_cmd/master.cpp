#include <glee_cmd/master.hpp>

#include <stdlib.h>
#include <stdio.h>

#include <boost/program_options.hpp>
#include <glee/api.hpp>
#include <glee_cmd/api.hpp>

namespace glee { namespace cmd {

namespace po = boost::program_options;

char const *_LIMIT_MUST_GT_THAN_ZERO = 
        "Limit number must be greater than zero";

static void _master_help(FILE *os)
{
        fprintf(os, "USAGE\n");
        fprintf(os, "   glee_master --help\n");
        fprintf(os, "   glee_master play [--limit, -l] [--keyword, -k]\n");
}

void master_help(void)
{
        _master_help(stdout);
        exit(EXIT_SUCCESS);
}

void master_failed_help(void)
{
        _master_help(stderr);
        exit(EXIT_FAILURE);
}

void master_play(int argc, char *argv[])
{
        po::options_description desc;
        desc.add_options()
                ("keyword,k", po::value<std::string>()->default_value(""),
                              "keyword to search")
                ("limit,l", po::value<size_t>()->default_value(0),
                           "number of file to play");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        glee::api api = glee::cmd::api();
        size_t limit = vm["limit"].as<size_t>();
        size_t iterator = 0;

        try {
                glee::media_file file;
                for (;;) {
                        file = api.master_find_media_file(
                                        vm["keyword"].as<std::string>(),
                                        iterator);
                        iterator += 1;
                        printf("%zu# %s\n", file.id,
                               limit_str_size(file.file_name, 60).c_str());
                        api.play_media_file(file.id);
                        if (limit > 0 && iterator >= limit) break;
                }
        } catch (const glee::index_error &e) {
        }

        exit(EXIT_SUCCESS);
}

} }
