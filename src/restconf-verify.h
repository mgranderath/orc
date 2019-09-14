#ifndef RESTCONF_VERIFY_H
#define RESTCONF_VERIFY_H
#include <json-c/json.h>
#include <uci/cmd.h>
#include "error.h"
#include "uci/methods.h"

UciWritePair **restconf_verify_leaf_list(struct json_object *content,
                                         struct json_object *yang_node,
                                         UciWritePair **command_list,
                                         error *err, int check_exists,
                                         struct UciPath *path);
error restconf_verify_leaf(struct json_object *value, struct UciPath *path,
                           struct json_object *yang, int check_exists);
#endif  // RESTCONF_VERIFY_H
