#include <glee_cmd/dir.hpp>

#include <stdio.h>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <glee_cmd/api.hpp>

namespace glee { namespace cmd {

namespace po = boost::program_options;

char const *_DIR_ID_MUST_BE_INT = "Directory's identity must be a number";
char const *_DIR_ID_NOT_EXIST = "Directory's identity is not exist";
char const *_DIR_NOT_EXIST = "Directory is not exist";

static void _dir_help(FILE *os) {
        fprintf(os, "USAGE\n");
        fprintf(os, "   glee_dir list [--keyword, -k kw] [--page,p p]\n");
        fprintf(os, "                 [--rows, -r]\n");
        fprintf(os, "   glee_dir add  <path>\n");
        fprintf(os, "   glee_dir del  <identity>\n");
        fprintf(os, "   glee_dir scan [identity]...\n");
        fprintf(os, "\n");
}

void dir_help() 
{ 
        _dir_help(stdout); 
        exit(EXIT_SUCCESS);
}

void dir_failed_help(void)
{
        _dir_help(stderr);
        exit(EXIT_FAILURE);
}

void dir_list(int argc, char *argv[])
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

        std::vector<media_dir> dirs;
        dirs = glee::cmd::api().search_media_dir(
                vm["keyword"].as<std::string>(),
                vm["page"].as<size_t>() - 1,
                vm["rows"].as<size_t>());
        for (auto it = dirs.begin(); it != dirs.end(); ++it)
                printf("#%-10zu %s\n", it->id, it->path.c_str());

        exit(EXIT_SUCCESS);
}

void dir_add(int argc, char *argv[])
{
        if (argc != 1)
                glee::cmd::dir_failed_help();
        glee::cmd::api().add_media_dir(argv[0]);

        exit(EXIT_SUCCESS);
}

void dir_del(int argc, char *argv[])
{
        size_t dir_id;

        if (argc != 1)
                glee::cmd::dir_failed_help();
        try { 
                dir_id = boost::lexical_cast<size_t>(argv[0]);
        } catch (const boost::bad_lexical_cast &e) {
                glee::cmd::exit_error(_DIR_ID_MUST_BE_INT);
        }
        glee::cmd::api().del_media_dir(dir_id);

        exit(EXIT_SUCCESS);
}

void dir_scan(int argc, char *argv[])
{
        size_t dir_id;
        glee::media_dir dir;
        
        if (argc < 1)
                glee::cmd::dir_failed_help();

        try {
                for (int i = 0; i < argc; ++i) {
                        dir_id = boost::lexical_cast<size_t>(argv[i]);
                        dir = glee::cmd::api().find_media_dir_by_id(dir_id);
                        std::cout << "Scan " << dir.path << "..." << std::endl;
                        glee::cmd::api().scan_by_dir_id(dir_id);
                }
        } catch (const boost::bad_lexical_cast &e) {
                glee::cmd::exit_error(_DIR_ID_MUST_BE_INT);
        } catch (const glee::sqlite_key_error &e) {
                glee::cmd::exit_error(_DIR_ID_NOT_EXIST);
        } catch (boost::filesystem::filesystem_error &e) {
                glee::cmd::exit_error(_DIR_NOT_EXIST);
        }
        
        exit(EXIT_SUCCESS);
}

} }
