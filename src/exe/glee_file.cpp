#include <glee_cmd/file.hpp>

#include <string.h>
#include <iostream>

int main(int argc, char *argv[])
{
        try {
                if (argc == 2 && !strcmp(argv[1], "--help"))
                        glee::cmd::file_help();
                else if (argc >= 2 && !strcmp(argv[1], "list"))
                        glee::cmd::file_list(argc - 1, argv + 1);
                else if (argc == 3 && !strcmp(argv[1], "play"))
                        glee::cmd::file_play(argc - 2, argv + 2);
                else
                        glee::cmd::file_failed_help();
        } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
        }
}
