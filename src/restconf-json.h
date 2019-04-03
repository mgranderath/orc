#ifndef _RESTCONF_JSON_H
#define _RESTCONF_JSON_H

#include <json.h>
#include "error.h"
#include "generated/yang.h"

#define json_array_forloop(array, index) \
  int index;                             \
  for (index = 0; index < json_object_array_length(array); index++)

const char* json_get_string(struct json_object* jobj, const char* key);
struct json_object* json_get_array(struct json_object* jobj, const char* key);
struct json_object* json_get_object_from_map(struct json_object* jobj,
                                             const char* key);
void json_pretty_print(struct json_object* jobj);
yang_type str_to_yang_type(const char* str);
struct json_object* json_yang_type_format(yang_type type, const char* val);
error json_yang_verify_list(struct json_object* list, struct json_object* yang);

#endif  //_RESTCONF_JSON_H
