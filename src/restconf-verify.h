#ifndef RESTCONF_VERIFY_H
#define RESTCONF_VERIFY_H
#include <json-c/json.h>
#include "error.h"
#include "uci/methods.h"

error restconf_verify_leaf_list(struct json_object *value,
                                struct uci_path *path, struct json_object *yang,
                                int check_exists);
error restconf_verify_leaf(struct json_object *value, struct uci_path *path,
                           struct json_object *yang, int check_exists);
#endif  // RESTCONF_VERIFY_H
