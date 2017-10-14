#ifndef GLEE_CMD_FILE_HPP
#define GLEE_CMD_FILE_HPP

#include <stdio.h>

namespace glee { namespace cmd {

void file_help(void);
void file_failed_help(void);
void file_list(int argc, char *argv[]);
void file_play(int argc, char *argv[]);

} }

#endif
