#include "uci-get.h"
#include "cmd.h"
#include "error.h"
#include "http.h"
#include "restconf-json.h"
#include "restconf-method.h"
#include "uci/uci-util.h"
#include "url.h"
#include "util.h"
#include "vector.h"

struct json_object *uci_get_list(struct json_object *yang,
                                 struct uci_path *path, error *err) {
  struct json_object *array = NULL;
  int list_length;
  int single_item = path->where;

  if (!json_get_string(yang, "type")) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  if (strlen(path->section_type) == 0 || strlen(path->section) != 0) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }

  struct json_object *map = NULL;
  json_object_object_get_ex(yang, "map", &map);

  if ((list_length = uci_list_length(path)) < 1) {
    *err = RE_OK;
    return NULL;
  }

  array = json_object_new_array();
  for (int index = 0; index < list_length; index++) {
    if (!single_item) {
      path->index = index;
      path->where = 1;
    }
    struct json_object *top_level = json_object_new_object();
    json_object_object_foreach(map, key, val) {
      error err_rec = RE_OK;
      struct json_object *check = build_recursive(val, path, &err_rec, 0);
      if (check) {
        json_object_object_add(top_level, key, check);
      }
    }
    json_object_array_add(array, top_level);
    if (single_item) {
      break;
    }
  }

  *err = RE_OK;
  path->index = 0;
  path->where = 0;
  if (json_object_array_length(array) == 0) {
    return NULL;
  }
  return array;
}

struct json_object *uci_get_leaf(struct json_object *yang,
                                 struct uci_path *path, error *err) {
  char path_string[512], buf[512];
  yang_type leaf_type;
  struct json_object *output = NULL;
  struct json_object *type = NULL;

  uci_combine_to_path(path, path_string, sizeof(path_string));
  if (uci_read_option(path_string, buf, sizeof(buf))) {
    *err = UCI_READ_FAILED;
    goto done;
  }
  json_object_object_get_ex(yang, "leaf-type", &type);
  if (!type) {
    *err = YANG_SCHEMA_ERROR;
    goto done;
  }
  if ((leaf_type = json_extract_yang_type(type)) == NONE) {
    *err = YANG_SCHEMA_ERROR;
    goto done;
  }
  if (!(output = json_yang_type_format(leaf_type, buf))) {
    *err = YANG_SCHEMA_ERROR;
    goto done;
  }
  *err = RE_OK;
done:
  path->option = "";
  return output;
}

struct json_object *uci_get_leaf_list(struct json_object *yang,
                                      struct uci_path *path, error *err) {
  char path_string[512];
  char **items = NULL;
  yang_type leaf_type;
  struct json_object *output = NULL;
  struct json_object *type = NULL;

  uci_combine_to_path(path, path_string, sizeof(path_string));
  if (!(items = uci_read_list(path_string))) {
    *err = UCI_READ_FAILED;
    goto done;
  }
  json_object_object_get_ex(yang, "leaf-type", &type);
  if (!type) {
    *err = YANG_SCHEMA_ERROR;
    goto done;
  }
  if ((leaf_type = json_extract_yang_type(type)) == NONE) {
    *err = YANG_SCHEMA_ERROR;
    goto done;
  }
  output = json_object_new_array();
  for (size_t i = 0; i < vector_size(items); i++) {
    struct json_object *iter = NULL;
    iter = json_yang_type_format(leaf_type, items[i]);
    if (!iter) {
      *err = YANG_SCHEMA_ERROR;
      goto done;
    }
    json_object_array_add(output, iter);
  }
  vector_free(items);
  *err = RE_OK;
done:
  path->option = "";
  return output;
}