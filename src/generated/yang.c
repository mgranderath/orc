#include "yang.h"
#include "yang-util.h"

/**
 * Check if YANG module exists
 * @param module name of the module
 * @return json_object* of the YANG module
 */
struct json_object *yang_module_exists(char *module) {
  const map_str2str *iter = modulemap;
  const map_str2str *end = modulemap + sizeof(modulemap) / sizeof(modulemap[0]);
  int found = 0;
  while (iter && iter < end) {
    if (strlen(module) == strlen(iter->key) && strcmp(module, iter->key) == 0) {
      found = 1;
      break;
    }
    iter++;
  }
  if (!found) {
    return NULL;
  }
  struct json_object *jobj = json_tokener_parse(iter->str);
  return jobj;
}

/**
 * Convert string of type to yang type
 * @param str the input type
 * @return yang_type of the str
 */
yang_type str_to_yang_type(const char *str) {
  if (!str) {
    return NONE;
  }
  struct json_object *tmp = NULL;
  const char *leaf_type = NULL;
  const map_str2yang_type *iter = str2yang_type;
  const map_str2yang_type *end =
      str2yang_type + sizeof(str2yang_type) / sizeof(str2yang_type[0]);
  while (iter && iter < end) {
    if (strlen(str) == strlen(iter->str) && strcmp(str, iter->str) == 0) {
      return iter->type;
    }
    iter++;
  }

  if (!(leaf_type = yang_for_type(str))) return NONE;
  struct json_object *yang = json_tokener_parse(leaf_type);
  json_object_object_get_ex(yang, YANG_LEAF_TYPE, &tmp);
  if (!tmp || json_object_get_type(tmp) != json_type_string) {
    return NONE;
  }
  yang_type final_value = str_to_yang_type(json_object_get_string(tmp));
  json_object_put(yang);
  return final_value;
}

/**
 * Getting yingson from typedef and imported types
 * @param type the input type
 * @return the json string
 */
const char *yang_for_type(const char *type) {
  const map_str2str *iter = yang2regex;
  const map_str2str *end =
      yang2regex + sizeof(yang2regex) / sizeof(yang2regex[0]);
  while (iter && iter < end) {
    if (strcmp(type, iter->key) == 0) {
      return iter->str;
    }
    iter++;
  }
  return NULL;
}