#ifndef GUI_GLEE_API_HPP
#define GUI_GLEE_API_HPP

#include <glee/api.hpp>

#define FILE_ROWS_PP 16

extern glee::api *glee_api;
void init_glee_api(void);
std::string time_to_str(const std::time_t *t);

#endif
