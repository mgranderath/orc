#ifndef RESTCONF_UCI_UTIL_
#define RESTCONF_UCI_UTIL_

#include "methods.h"

int combine_to_path(struct uci_path *path, char *buffer, size_t size);
int combine_to_anonymous_path(struct uci_path *path, int index, char *buffer,
                              size_t size);
int uci_combine_to_path(struct uci_path *path, char *buffer,
                        size_t buffer_size);
int get_path_from_yang(struct json_object *jobj, struct uci_path *uci);
#endif  // RESTCONF_UCI_UTIL_
