#include "restconf-json.h"
#include <stdio.h>
#include "error.h"
#include "restconf.h"
#include "yang-util.h"

/**
 * Return json_object's value as a string
 * @param jobj input object to be searched
 * @param key the key of the value
 * @return char* of the string
 */
const char* json_get_string(struct json_object* jobj, const char* key) {
  struct json_object* tmp = NULL;
  json_object_object_get_ex(jobj, key, &tmp);
  if (!tmp || json_object_get_type(tmp) != json_type_string) {
    return NULL;
  }
  return json_object_get_string(tmp);
}

struct json_object* json_get_array(struct json_object* jobj, const char* key) {
  struct json_object* tmp = NULL;
  json_object_object_get_ex(jobj, key, &tmp);
  if (!tmp || json_object_get_type(tmp) != json_type_array) {
    return NULL;
  }
  return tmp;
}

/**
 * Return json_object from "map" with key
 * @param jobj input object to be searched
 * @param key the key of the value in "map"
 * @return json_object with the value
 */
struct json_object* json_get_object_from_map(struct json_object* jobj,
                                             const char* key) {
  json_object* tmp = NULL;
  json_object_object_get_ex(jobj, YANG_MAP, &tmp);
  json_object_object_get_ex(tmp, key, &tmp);
  return tmp;
}

/**
 * Pretty prints a json_object
 * @param jobj the json_object to be printed
 */
void json_pretty_print(struct json_object* jobj) {
  printf("%s\n", json_object_to_json_string_ext(
                     jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}

/**
 * Converts a string with a type to a json_object
 * @param type the yang_type of the string
 * @param val a string to be converted
 * @return json_object with correct type
 */
struct json_object* json_yang_type_format(yang_type type, const char* val) {
  if (type == INT_8 || type == INT_16 || type == INT_32 || type == UINT_8 ||
      type == UINT_16 || type == UINT_32) {
    int32_t integer = strtoimax(val, NULL, 10);
    return json_object_new_int(integer);
  }
  if (type == STRING || type == INT_64 || type == UINT_64) {
    return json_object_new_string(val);
  }
  return NULL;
}

/**
 * @brief extract key values from JSON item
 * @param keys the list of keys
 * @param item the JSON item
 * @param ret the object the key-value pairs should be copied to
 * @return error in case of error
 */
error json_extract_key_values(struct json_object* keys,
                              struct json_object* item,
                              struct json_object** ret) {
  struct json_object* values = json_object_new_object();
  for (size_t keys_i = 0; keys_i < json_object_array_length(keys); keys_i++) {
    const char* key = NULL;
    struct json_object* key_value = NULL;
    if ((key = json_object_get_string(
             json_object_array_get_idx(keys, keys_i))) == NULL) {
      return YANG_SCHEMA_ERROR;
    }
    json_object_object_get_ex(item, key, &key_value);
    if (key_value == NULL) {
      return KEY_NOT_PRESENT;
    }
    json_object_object_add(values, key, key_value);
  }
  *ret = values;
  return RE_OK;
}

/**
 * @brief check mandatory items of an JSON item
 * @param mandatory the list of names of mandatory values
 * @param item the JSON item to be checked
 * @return error in case of error otherwise RE_OK
 */
static error check_mandatory_values(struct json_object* mandatory,
                                    struct json_object* item) {
  for (size_t mandatory_i = 0;
       mandatory_i < json_object_array_length(mandatory); mandatory_i++) {
    const char* key = NULL;
    struct json_object* key_value = NULL;
    if ((key = json_object_get_string(
             json_object_array_get_idx(mandatory, mandatory_i))) == NULL) {
      return YANG_SCHEMA_ERROR;
    }
    json_object_object_get_ex(item, key, &key_value);
    if (key_value == NULL) {
      return MANDATORY_NOT_PRESENT;
    }
  }
  return RE_OK;
}

/**
 * @brief verify a JSON list against a YANG list node
 * @param list the JSON list
 * @param yang the YANG list node
 * @return error in case of error else RE_OK
 */
error json_yang_verify_list(struct json_object* list,
                            struct json_object* yang) {
  int keys_exist = 1;
  int mandatory_exist = 1;
  int unique_exist = 1;
  int leaf_as_name_exist = 1;
  struct json_object* keys = NULL;
  struct json_object* mandatory = NULL;
  struct json_object* unique = NULL;

  if (!json_is_array(json_object_get_type(list))) {
    return INVALID_TYPE;
  }

  if (!(keys = json_get_array(yang, YANG_KEYS))) {
    // No keys defined in YANG
    keys_exist = 0;
  }
  if (!(unique = json_get_array(yang, YANG_UNIQUE))) {
    unique_exist = 0;
  }
  if (!(mandatory = json_get_array(yang, YANG_MANDATORY))) {
    mandatory_exist = 0;
  }
  for (size_t list_i = 0; list_i < json_object_array_length(list); list_i++) {
    struct json_object* key_values = NULL;
    struct json_object* unique_values = NULL;
    struct json_object* list_item = json_object_array_get_idx(list, list_i);
    if (keys_exist) {
      if (json_extract_key_values(keys, list_item, &key_values) != RE_OK) {
        return KEY_NOT_PRESENT;
      }
    }

    if (unique_exist) {
      if (json_extract_key_values(unique, list_item, &unique_values) != RE_OK) {
        return KEY_NOT_PRESENT;
      }
    }

    if (mandatory_exist) {
      if (check_mandatory_values(mandatory, list_item) != RE_OK) {
        return MANDATORY_NOT_PRESENT;
      }
    }

    for (size_t verify_i = list_i + 1;
         verify_i < json_object_array_length(list) && keys_exist; verify_i++) {
      int different = 0;
      json_object_object_foreach(key_values, key, value) {
        struct json_object* object_key_value = NULL;
        const char* key_value = json_object_get_string(value);
        struct json_object* list_item_verify =
            json_object_array_get_idx(list, verify_i);
        json_object_object_get_ex(list_item_verify, key, &object_key_value);
        if (key_value == NULL) {
          return KEY_NOT_PRESENT;
        }
        const char* object_key_value_string =
            json_object_get_string(object_key_value);
        if (strcmp(key_value, object_key_value_string) != 0) {
          different = 1;
        }
      }
      if (!different) {
        return IDENTICAL_KEYS;
      }
      different = 0;
      if (!unique_exist) {
        continue;
      }
      json_object_object_foreach(unique_values, uniqueKey, uniqueValue) {
        struct json_object* object_unique_value = NULL;
        const char* key_value = json_object_get_string(uniqueValue);
        struct json_object* list_item_verify =
            json_object_array_get_idx(list, verify_i);
        json_object_object_get_ex(list_item_verify, uniqueKey,
                                  &object_unique_value);
        if (key_value == NULL) {
          return KEY_NOT_PRESENT;
        }
        const char* object_key_value_string =
            json_object_get_string(object_unique_value);
        if (strcmp(key_value, object_key_value_string) != 0) {
          different = 1;
        }
      }
      if (!different) {
        return IDENTICAL_KEYS;
      }
    }
  }

  return RE_OK;
}

/**
 * @brief check if value in JSON array
 * @param array the JSON array
 * @param value the value to be compared
 * @return 1 if in array else 0
 */
int json_value_in_array(struct json_object* array, char* value) {
  for (size_t i = 0; i < json_object_array_length(array); i++) {
    const char* item_value = NULL;
    if ((item_value = json_object_get_string(
             json_object_array_get_idx(array, i))) == NULL) {
      return -1;
    }
    if (strcmp(item_value, value) == 0) {
      return 1;
    }
  }
  return 0;
}

/**
 * @brief extract YANG type from JSON node
 * @param item the YANG JSON node
 * @return the yang_type
 */
yang_type json_extract_yang_type(struct json_object* item) {
  const char* leaf_type = NULL;
  if (json_object_get_type(item) == json_type_object) {
    leaf_type = json_get_string(item, YANG_LEAF_TYPE);
  } else {
    leaf_type = json_object_get_string(item);
  }
  if (!leaf_type) {
    return NONE;
  }
  return str_to_yang_type(leaf_type);
}

int json_is_object(json_type type) { return type == json_type_object; }

int json_is_array(json_type type) { return type == json_type_array; }