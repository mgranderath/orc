#include "yang-verify.h"
#include "restconf-json.h"

error yang_verify_leaf(struct json_object* leaf, struct json_object* yang) {
  yang_type yang_leaf_type;
  const char* yang_type_string = NULL;
  json_type value_type = json_object_get_type(leaf);

  if (value_type == json_type_object || value_type == json_type_array) {
    return INVALID_TYPE;
  }

  yang_type_string = json_get_string(yang, "leaf-type");
  yang_leaf_type = str_to_yang_type(yang_type_string);
  if (yang_verify_json_type(yang_leaf_type, value_type)) {
    return INVALID_TYPE;
  }
  return RE_OK;
}

error yang_verify_leaf_list(struct json_object* list,
                            struct json_object* yang) {
  yang_type yang_leaf_list_type;
  const char* yang_type_string = NULL;
  json_type value_type = json_object_get_type(list);
  if (value_type != json_type_array) {
    return INVALID_TYPE;
  }

  yang_type_string = json_get_string(yang, "leaf-type");
  yang_leaf_list_type = str_to_yang_type(yang_type_string);
  for (int i = 0; i < json_object_array_length(list); i++) {
    json_type element_type =
        json_object_get_type(json_object_array_get_idx(list, i));
    if (yang_verify_json_type(yang_leaf_list_type, element_type)) {
      return INVALID_TYPE;
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