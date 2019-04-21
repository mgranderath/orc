#include "yang-verify.h"
#include <regex.h>
#include "restconf-json.h"

error yang_verify_leaf(struct json_object* leaf, struct json_object* yang) {
  const char* value = NULL;
  struct json_object* type = NULL;
  json_type value_type = json_object_get_type(leaf);

  if (value_type == json_type_object || value_type == json_type_array) {
    return INVALID_TYPE;
  }

  json_object_object_get_ex(yang, "leaf-type", &type);
  if (!type) {
    return YANG_SCHEMA_ERROR;
  }

  if (!(value = json_object_get_string(leaf))) {
    return INVALID_TYPE;
  }
  if (yang_verify_value_type(type, value)) {
    return INVALID_TYPE;
  }
  return RE_OK;
}

error yang_verify_leaf_list(struct json_object* list,
                            struct json_object* yang) {
  struct json_object* type = NULL;
  json_type value_type = json_object_get_type(list);
  if (value_type != json_type_array) {
    return INVALID_TYPE;
  }

  json_object_object_get_ex(yang, "leaf-type", &type);
  if (!type) {
    return YANG_SCHEMA_ERROR;
  }
  for (int i = 0; i < json_object_array_length(list); i++) {
    const char* value = NULL;
    struct json_object* item = json_object_array_get_idx(list, i);
    if (!(value = json_object_get_string(item))) {
      return INVALID_TYPE;
    }
    if (yang_verify_value_type(type, value)) {
      return INVALID_TYPE;
    }
    for (int compare = i + 1; compare < json_object_array_length(list);
         compare++) {
      const char* compare_value = NULL;
      struct json_object* compare_item =
          json_object_array_get_idx(list, compare);
      if (!(compare_value = json_object_get_string(compare_item))) {
        return INVALID_TYPE;
      }
      if (strcmp(value, compare_value) == 0) {
        return IDENTICAL_KEYS;
      }
    }
  }
  return RE_OK;
}

int yang_verify_json_type(yang_type type, json_type val_type) {
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
      if (val_type != json_type_string && val_type != json_type_int) return 1;
      break;
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

int yang_mandatory(struct json_object* yang) {
  const char* type_string = NULL;
  type_string = json_get_string(yang, "type");
  if (!type_string || strcmp(type_string, "leaf") == 0) {
    struct json_object* mandatory = NULL;
    json_object_object_get_ex(yang, "mandatory", &mandatory);
    if (mandatory && json_object_get_boolean(mandatory)) {
      return 1;
    }
  }
  return 0;
}

int regex_verify_value(const char* regex, const char* value) {
  regex_t regex_o;
  if (regcomp(&regex_o, regex, REG_EXTENDED)) {
    return 1;
  }
  if (regexec(&regex_o, value, 0, NULL, 0)) {
    regfree(&regex_o);
    return 1;
  }
  regfree(&regex_o);
  return 0;
}

int verify_value_from_imported(const char* type, const char* value) {
  enum json_tokener_error err;
  const char* typedefinition = yang_for_type((char*)type);
  if (!typedefinition) {
    return 1;
  }
  struct json_object* type_object =
      json_tokener_parse_verbose(typedefinition, &err);
  if (!type_object) {
    return 1;
  }
  int wrong_format = yang_verify_value_type(type_object, value);
  if (wrong_format) {
    json_object_put(type_object);
    return 1;
  }
  json_object_put(type_object);
  return 0;
}

int yang_verify_value_type(struct json_object* type, const char* value) {
  const char* leaf_type = NULL;
  int is_object = 0;
  enum other_check { NONE, RANGE, PATTERN } verify_type = NONE;
  if (json_object_get_type(type) == json_type_object) {
    leaf_type = json_get_string(type, "leaf-type");
    is_object = 1;
  } else {
    leaf_type = json_object_get_string(type);
  }
  yang_type converted = str_to_yang_type(leaf_type);
  switch (converted) {
    case BOOLEAN:
      if (strcmp(value, "true") != 0 && strcmp(value, "false") != 0) return 1;
      break;
    case EMPTY:
      break;
    case IDENTITY_REF:
      break;
    case INT_8: {
      verify_type = RANGE;
      int integer = strtoimax(value, NULL, 10);
      if (integer < -128 || integer > 127) return 1;
      break;
    }
    case INT_16: {
      verify_type = RANGE;
      int integer = strtoimax(value, NULL, 10);
      if (integer < -32768 || integer > 32767) return 1;
      break;
    }
    case INT_32: {
      verify_type = RANGE;
      int integer = strtoimax(value, NULL, 10);
      if (integer < -2147483648 || integer > 2147483647) return 1;
      break;
    }
    case UINT_8: {
      verify_type = RANGE;
      int integer = strtoimax(value, NULL, 10);
      if (integer < 0 || integer > 255) return 1;
      break;
    }
    case UINT_16: {
      verify_type = RANGE;
      int integer = strtoimax(value, NULL, 10);
      if (integer < 0 || integer > 65535) return 1;
      break;
    }
    case UINT_32: {
      verify_type = RANGE;
      int integer = strtoimax(value, NULL, 10);
      if (integer < 0) return 1;
      break;
    }
    case LEAF_REF:
      break;
    case UINT_64: {
      verify_type = RANGE;
      unsigned long integer = strtol(value, NULL, 10);
      if (integer < 0 || integer > 18446744073709551615u) return 1;
      break;
    }
    case INT_64:
      verify_type = RANGE;
      break;
    case DECIMAL_64:
    case ENUMERATION:
    case BITS:
    case INSTANCE_IDENTIFIER:
      break;
    case STRING:
      verify_type = PATTERN;
      break;
    case UNION:
      break;
    default:
      return verify_value_from_imported(leaf_type, value);
  }
  if (is_object) {
    switch (verify_type) {
      case RANGE: {
        const char* from_string = NULL;
        const char* to_string = NULL;
        from_string = json_get_string(type, "from");
        to_string = json_get_string(type, "to");
        if (!from_string || !to_string) {
          break;
        }
        long from = strtol(from_string, NULL, 10);
        long to = strtol(to_string, NULL, 10);
        long integer = strtol(value, NULL, 10);
        if (integer < from || integer > to) {
          return 1;
        }
        break;
      }
      case PATTERN: {
        const char* pattern = json_get_string(type, "pattern");
        if (!pattern) {
          break;
        }
        if (regex_verify_value(pattern, value)) {
          return 1;
        }
        break;
      }
      default:
        break;
    }
  }
  return 0;
}