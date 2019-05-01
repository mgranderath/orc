#ifndef RESTCONF_YANG_VERIFY_H
#define RESTCONF_YANG_VERIFY_H

#include <generated/yang.h>
#include <json.h>
#include "error.h"

error yang_verify_leaf(struct json_object* leaf, struct json_object* yang);
error yang_verify_leaf_list(struct json_object* list, struct json_object* yang);
int yang_verify_json_type(yang_type type, json_type val_type);
int yang_mandatory(struct json_object* yang);

#endif  // RESTCONF_YANG_VERIFY_H
