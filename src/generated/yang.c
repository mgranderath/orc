#include "yang.h"

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
  const map_str2yang_type *iter = str2yang_type;
  const map_str2yang_type *end =
      str2yang_type + sizeof(str2yang_type) / sizeof(str2yang_type[0]);
  while (iter && iter < end) {
    if (strlen(str) == strlen(iter->str) && strcmp(str, iter->str) == 0) {
      return iter->type;
    }
    iter++;
  }
  return NONE;
}