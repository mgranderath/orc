#ifndef RESTCONF_UCI_UTIL_
#define RESTCONF_UCI_UTIL_

#include "methods.h"

int combine_to_path(struct UciPath *path, char *buffer, size_t size);
int combine_to_anonymous_path(struct UciPath *path, int index, char *buffer,
                              size_t size);
int uci_combine_to_path(struct UciPath *path, char *buffer, size_t buffer_size);
int get_path_from_yang(struct json_object *jobj, struct UciPath *uci);
int get_leaf_as_name(struct json_object *yang, struct json_object *json,
                     struct UciPath *uci);
int uci_revert_all(char **package_list);
int uci_commit_all(char **package_list);
int uci_element_exists(struct UciPath *path);

#endif  // RESTCONF_UCI_UTIL_
