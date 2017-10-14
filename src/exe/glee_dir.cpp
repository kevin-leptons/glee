#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glee_cmd/dir.hpp>

int main(int argc, char *argv[])
{
        if (argc == 2 && !strcmp(argv[1], "--help")) {
                glee::cmd::dir_help();
        } else if (argc == 3 && !strcmp(argv[1], "add")) {
                glee::cmd::dir_add(argc - 2, argv + 2);
        } else if (argc == 3 && !strcmp(argv[1], "del")) {
                glee::cmd::dir_del(argc - 2, argv + 2);
        } else if (argc >= 2 && !strcmp(argv[1], "list")) {
                glee::cmd::dir_list(argc - 1, argv + 1);
        } else if (argc >= 3 && !strcmp(argv[1], "scan")) {
                glee::cmd::dir_scan(argc - 2, argv + 2);
        } else {
                glee::cmd::dir_failed_help();
        }
}
