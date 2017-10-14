#ifndef GLEE_CMD_API_HPP
#define GLEE_CMD_API_HPP

#include <glee/api.hpp>
#include <string>

namespace glee { namespace cmd {

#define ROWS_PER_PAGE 16

extern char const *PAGE_NUMBER_MUST_BE_GT_THAN_ZERO;

glee::api api(void);
void exit_error(const std::string &message);
std::string limit_str_size(const std::string &str, size_t limit_size);

} }

#endif
