#ifndef RESTCONF_UTIL_H
#define RESTCONF_UTIL_H
#include <stddef.h>
#include "cgi.h"

int split_pair_by_char(char *split, char **first, char **second,
                       char split_char);
char *str_dup(const char *c);
char *strn_dup(const char *c, size_t to);

int is_in_vector(char **vec, char *value);
#endif  // RESTCONF_UTIL_H
