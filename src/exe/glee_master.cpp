#include <glee_cmd/master.hpp>

#include <string.h>

int main(int argc, char *argv[])
{
        if (argc == 2 && !strcmp(argv[1], "--help"))
                glee::cmd::master_help();
        else if (argc >= 2 && !strcmp(argv[1], "play"))
                glee::cmd::master_play(argc - 1, argv + 1);
        else
                glee::cmd::master_failed_help();
}
