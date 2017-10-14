#include <glee_cmd/file.hpp>

#include <stdlib.h>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <glee_cmd/api.hpp>

namespace glee { namespace cmd {

namespace po = boost::program_options;

char const *_FILE_ID_MUST_BE_INT = "File's identity must be number";

static void _file_help(FILE *os)
{
        fprintf(os, "USAGE\n");
        fprintf(os, "   glee_file list [--keyword, -k] [--page, -p]\n");
        fprintf(os, "                  [--rows, -r]\n");
        fprintf(os, "   glee_file open <file_id>\n");
}

void file_help(void)
{
        _file_help(stdout);
        exit(EXIT_SUCCESS);
}

void file_failed_help(void)
{
        _file_help(stderr);
        exit(EXIT_FAILURE);
}

void file_list(int argc, char *argv[])
{

        po::options_description desc;
        desc.add_options()
                ("keyword,k", po::value<std::string>()->default_value(""),
                              "keyword to search")
                ("page,p", po::value<size_t>()->default_value(1),
                           "page number")
                ("rows,r", po::value<size_t>()->default_value(ROWS_PER_PAGE),
                           "number of rows for page");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm["page"].as<size_t>() == 0)
                exit_error(PAGE_NUMBER_MUST_BE_GT_THAN_ZERO);

        std::vector<glee::media_file> files;
        files = glee::cmd::api().search_media_file(
                vm["keyword"].as<std::string>(),
                vm["page"].as<size_t>() - 1,
                vm["rows"].as<size_t>());
        for (auto it = files.begin(); it != files.end(); ++it)
                printf("%zu# %s\n", it->id,
                       limit_str_size(it->file_name, 60).c_str());

        exit(EXIT_SUCCESS);
}

void file_play(int argc, char *argv[])
{
        if (argc != 1)
                glee::cmd::file_failed_help();

        try {
                glee::api api = glee::cmd::api();
                size_t file_id = boost::lexical_cast<size_t>(argv[0]);
                glee::media_file file = api.find_media_file_by_id(file_id);
                printf("Play: %zu# %s\n", file.id, 
                        limit_str_size(file.file_name, 60).c_str());
                api.play_media_file(file_id);
        } catch (const boost::bad_lexical_cast &e) {
                std::cout << "Error: " << _FILE_ID_MUST_BE_INT << std::endl;
        } catch (const glee::sqlite_key_error &e) {
                fprintf(stderr, "Error: Identity not found '%s'\n",
                        e.what());
        }

        exit(EXIT_SUCCESS);
} 

}}
