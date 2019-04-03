#ifndef RESTCONF_UCI_GET_H
#define RESTCONF_UCI_GET_H

#include "cgi.h"
#include "error.h"
#include "uci/methods.h"

struct json_object *uci_get_list(struct json_object *yang,
                                 struct uci_path *path, error *err);
struct json_object *uci_get_leaf(struct json_object *yang,
                                 struct uci_path *path, error *err);
struct json_object *uci_get_leaf_list(struct json_object *yang,
                                      struct uci_path *path, error *err);

#endif  // RESTCONF_UCI_GET_H
