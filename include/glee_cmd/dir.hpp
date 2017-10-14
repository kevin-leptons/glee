#ifndef GLEE_CMD_DIR_HPP
#define GLEE_CMD_DIR_HPP

namespace glee { namespace cmd {

/*
        Show help information in stdout.
*/
void dir_help(void);

/*
        Show help information in stderr then exit program with
        EXIT_FAILURE.
*/
void dir_failed_help(void);

/*
        Search and show directory path.

FORMAT: .. list [keyword]
*/
void dir_list(int argc, char *argv[]);

/*
        Add directory into database. If directory is early exists in database
        or directory is not exist in filesystem, message that and exit program
        with EXIT_FAILURE.

FORMAT: .. add <dir_path>
*/
void dir_add(int argc, char *argv[]);

/*
        Remove director from database. If director is not early exists in
        database, message that and exit program with EXIT_FAILURE.

FORMAT: .. del <dir_id>
*/
void dir_del(int argc, char *argv[]);

/*
        Scan specific directories by it's identity.

FORMAT: .. scan [dir_id]...
*/
void dir_scan(int argc, char *argv[]);

} }

#endif
