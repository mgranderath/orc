#ifndef RESTCONF_UTIL_H
#define RESTCONF_UTIL_H
#include <stddef.h>
#include "cgi.h"

int split_module_top_level(char *split, char **module, char **top_level);
char *str_dup(const char *c);
char *strn_dup(const char *c, size_t to);
#endif  // RESTCONF_UTIL_H
