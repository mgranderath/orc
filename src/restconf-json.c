#include "restconf-json.h"
#include <stdio.h>
#include "error.h"

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
  json_object_object_get_ex(jobj, "map", &tmp);
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

int verify_yang_json_type(yang_type type, json_type val_type) {
  switch (type) {
    case BOOLEAN:
      if (val_type != json_type_boolean) return 1;
      break;
    case EMPTY:
      if (val_type != json_type_null) return 1;
      break;
    case IDENTITY_REF:
      break;
    case INT_8:
    case INT_16:
    case INT_32:
    case UINT_8:
    case UINT_16:
    case UINT_32:
      if (val_type != json_type_int) return 1;
      break;
    case LEAF_REF:
      break;
    case UINT_64:
    case INT_64:
    case DECIMAL_64:
    case ENUMERATION:
    case BITS:
    case BINARY:
    case STRING:
    case INSTANCE_IDENTIFIER:
      if (val_type != json_type_string) return 1;
      break;
    case UNION:
      break;
    default:
      return 0;
  }
  return 0;
}

error json_yang_verify_list(struct json_object* list,
                            struct json_object* yang) {
  struct json_object* keys = NULL;

  if (json_object_get_type(list) != json_type_array) {
    return INVALID_TYPE;
  }
  if ((keys = json_get_array(yang, "keys")) == NULL) {
    // No keys defined in YANG
    return YANG_SCHEMA_ERROR;
  }
  for (size_t list_i = 0; list_i < json_object_array_length(list); list_i++) {
    struct json_object* values = json_object_new_object();
    for (size_t keys_i = 0; keys_i < json_object_array_length(keys); keys_i++) {
      const char* key = NULL;
      struct json_object* key_value = NULL;
      if ((key = json_object_get_string(
               json_object_array_get_idx(keys, keys_i))) == NULL) {
        return YANG_SCHEMA_ERROR;
      }
      struct json_object* list_item = json_object_array_get_idx(list, list_i);
      json_object_object_get_ex(list_item, key, &key_value);
      if (key_value == NULL) {
        return KEY_NOT_PRESENT;
      }
      // Verify that key is leaf
      json_object_object_add(values, key, key_value);
    }

    for (size_t verify_i = list_i + 1;
         verify_i < json_object_array_length(list); verify_i++) {
      int different = 0;
      json_object_object_foreach(values, key, value) {
        struct json_object* object_key_value = NULL;
        const char* key_value = json_object_get_string(value);
        struct json_object* list_item =
            json_object_array_get_idx(list, verify_i);
        json_object_object_get_ex(list_item, key, &object_key_value);
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
    }
  }

  return RE_OK;
}