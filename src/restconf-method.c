#include "restconf-method.h"
#include "error.h"
#include "http.h"
#include "restconf-json.h"
#include "restconf-verify.h"
#include "uci/cmd.h"
#include "uci/uci-get.h"
#include "uci/uci-util.h"
#include "url.h"
#include "util.h"
#include "vector.h"
#include "yang-verify.h"

uci_write_pair **verify_content_yang(struct json_object *content,
                                     struct json_object *yang_node,
                                     struct uci_path *path, error *err,
                                     int root, int check_exists) {
  uci_write_pair **command_list = NULL;
  const char *child_type = NULL;
  if (!(child_type = json_get_string(yang_node, "type"))) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  if (strcmp(child_type, "leaf-list") == 0) {
    json_type value_type = json_object_get_type(content);
    if (value_type == json_type_object) {
      *err = INVALID_TYPE;
      return NULL;
    }
    struct json_object *existing_items = NULL;
    if (check_exists) {
      if (!(existing_items = uci_get_leaf_list(yang_node, path, err))) {
        existing_items = json_object_new_array();
      }
    } else {
      existing_items = json_object_new_array();
    }
    if (*err != RE_OK && *err != UCI_READ_FAILED) {
      return NULL;
    }
    if (value_type == json_type_array) {
      json_array_forloop(content, index) {
        struct json_object *tmp = json_object_array_get_idx(content, index);
        json_object_array_add(existing_items, tmp);
      }
    } else {
      json_object_array_add(existing_items, content);
    }
    if ((*err = restconf_verify_leaf_list(existing_items, path, yang_node,
                                          0)) != RE_OK) {
      return NULL;
    }
    json_array_forloop(existing_items, index) {
      json_object *item = json_object_array_get_idx(existing_items, index);
      const char *value = json_object_get_string(item);
      uci_write_pair *output =
          initialize_uci_write_pair(path, (char *)value, list);
      if (!output) {
        *err = INTERNAL;
        return NULL;
      }
      vector_push_back(command_list, output);
    }
    return command_list;
  } else if (strcmp(child_type, "leaf") == 0) {
    if ((*err = restconf_verify_leaf(content, path, yang_node, check_exists)) !=
        RE_OK) {
      return NULL;
    }
    uci_write_pair *output = initialize_uci_write_pair(
        path, (char *)json_object_get_string(content), option);
    if (!output) {
      *err = INTERNAL;
      return NULL;
    }
    vector_push_back(command_list, output);
    return command_list;
  } else if (strcmp(child_type, "list") == 0 && path->where == 0) {
    json_type value_type = json_object_get_type(content);
    if (value_type != json_type_array && value_type != json_type_object) {
      *err = INVALID_TYPE;
      return NULL;
    }
    int list_length = uci_list_length(path);
    if (root) {
      if (value_type != json_type_object) {
        *err = MULTIPLE_OBJECTS;
        return NULL;
      }
      struct json_object *existing_values = NULL;
      if (check_exists) {
        existing_values = uci_get_list(yang_node, path, err);
        if (*err != RE_OK) {
          return NULL;
        }
      }
      if (!existing_values) {
        existing_values = json_object_new_array();
      }
      // combine with uci values when root is list
      if (value_type == json_type_array) {
        json_array_forloop(content, index) {
          struct json_object *item = json_object_array_get_idx(content, index);
          if (!item) {
            *err = INTERNAL;
            return NULL;
          }
          json_object_array_add(existing_values, item);
        }
      } else {
        json_object_array_add(existing_values, content);
      }
      *err = json_yang_verify_list(existing_values, yang_node);
      if (*err != RE_OK) {
        return NULL;
      }
    } else {
      if (list_length != 0 && check_exists) {
        *err = ELEMENT_ALREADY_EXISTS;
        return NULL;
      }

      *err = json_yang_verify_list(content, yang_node);
      if (*err != RE_OK) {
        return NULL;
      }
    }
    if (value_type == json_type_array) {
      int start = (path->where || !root) ? path->index : list_length;
      for (int index = 0, pos = start;
           index < json_object_array_length(content); index++, pos++) {
        struct json_object *tmp = json_object_array_get_idx(content, index);
        path->index = pos;
        path->where = 1;
        uci_write_pair **tmp_list =
            verify_content_yang(tmp, yang_node, path, err, 0, check_exists);
        if (*err != RE_OK) {
          free_uci_write_list(tmp_list);
          return NULL;
        }
        for (size_t i = 0; i < vector_size(tmp_list); i++) {
          vector_push_back(command_list, tmp_list[i]);
        }
        vector_free(tmp_list);
      }
    } else {
      path->index = (path->where || !root) ? path->index : list_length;
      path->where = 1;
      uci_write_pair **tmp_list =
          verify_content_yang(content, yang_node, path, err, 0, check_exists);
      if (*err != RE_OK) {
        free_uci_write_list(tmp_list);
        return NULL;
      }
      for (size_t i = 0; i < vector_size(tmp_list); i++) {
        vector_push_back(command_list, tmp_list[i]);
      }
      vector_free(tmp_list);
    }
    path->index = 0;
    path->where = 0;
    return command_list;
  }
  json_object_object_foreach(content, key, val) {
    struct json_object *child = NULL;
    char *module = NULL;
    char *split_key = NULL;
    if (split_pair_by_char(key, &module, &split_key, ':')) {
      split_key = key;
    }
    if (module) {
      free(module);
    }
    child = json_get_object_from_map(yang_node, split_key);
    free(split_key);
    if (!child) {
      *err = NO_SUCH_ELEMENT;
      return NULL;
    }
    get_path_from_yang(child, path);
    uci_write_pair **tmp_list =
        verify_content_yang(val, child, path, err, root, check_exists);
    if (*err != RE_OK) {
      free_uci_write_list(tmp_list);
      return NULL;
    }
    for (size_t i = 0; i < vector_size(tmp_list); i++) {
      vector_push_back(command_list, tmp_list[i]);
    }
    vector_free(tmp_list);
  }
  *err = RE_OK;
  return command_list;
}

error get_list_item_where(struct json_object *yang, char *keylist,
                          struct uci_path *uci) {
  struct json_object *keys = NULL;
  int array_length;

  if (!(keys = json_get_array(yang, "keys"))) {
    return YANG_SCHEMA_ERROR;
  }
  array_length = json_object_array_length(keys);
  rvec keylist_vec = clist_to_vec(keylist);
  if (!keylist_vec || array_length != vector_size(keylist_vec)) {
    // not as many keys as keys specified
    return LIST_UNDEFINED_KEY;
  }
  map_str2str key_value[array_length];
  for (int index = 0; index < array_length; index++) {
    struct json_object *key = NULL;
    struct json_object *key_child = NULL;
    const char *uci_option_name = NULL;
    const char *yang_option_name = NULL;

    key = json_object_array_get_idx(keys, index);
    if (!key || json_object_get_type(key) != json_type_string) {
      return YANG_SCHEMA_ERROR;
    }
    yang_option_name = json_object_get_string(key);
    key_child = json_get_object_from_map(yang, yang_option_name);
    if (!key_child) {
      return LEAF_NO_OPTION;
    }
    uci_option_name = json_get_string(key_child, "uci-option");
    if (uci_option_name == NULL) {
      return LEAF_NO_OPTION;
    }
    key_value[index].key = (char *)uci_option_name;
    key_value[index].str = keylist_vec[index];
  }
  struct uci_where where = {
      .path = uci, .key_value = key_value, .key_value_length = array_length};
  int where_index = uci_index_where(&where);
  if (where_index == -1) {
    uci->index = uci_list_length(uci);
    uci->where = 1;
    return LIST_UNDEFINED_KEY;
  }

  uci->where = 1;
  uci->index = where_index;
  vector_free(keylist_vec);
  return RE_OK;
}

error check_path(struct json_object **root_yang, char **path, size_t start,
                 size_t end, struct uci_path *uci, int check_keys,
                 int stop_at_key) {
  struct json_object *iter = *root_yang;
  struct json_object *keys = NULL;
  size_t i;
  if (!path) {
    return INTERNAL;
  }
  for (i = start; i < end; i++) {
    char *path_computed = NULL;
    char *obj = NULL;
    char *keylist = NULL;
    char *keylist_encoded = NULL;
    const char *type = NULL;
    struct json_object *child = NULL;
    error err;

    split_pair_by_char(path[i], &obj, &keylist_encoded, '=');
    if (obj && keylist_encoded) {
      keylist = malloc(strlen(keylist_encoded) + 1);
      urldecode(keylist, keylist_encoded);
      free(keylist_encoded);
    }

    if (!(path_computed = obj)) {
      path_computed = path[i];
    }

    if (check_keys && keys && json_value_in_array(keys, path_computed)) {
      return DELETING_KEY;
    }
    keys = NULL;

    if (!(child = json_get_object_from_map(iter, path_computed))) {
      return NO_SUCH_ELEMENT;
    }
    get_path_from_yang(child, uci);
    type = json_get_string(child, "type");
    if (type && strcmp(type, "list") == 0) {
      int next_is_end = i + 1 == end;
      if (!keylist && !next_is_end) {
        return LIST_NO_FILTER;
      } else if (!keylist) {
        return LIST_NO_FILTER;
      }

      if ((err = get_list_item_where(child, keylist, uci)) != RE_OK) {
        if ((err != LIST_UNDEFINED_KEY && !stop_at_key) || !stop_at_key) {
          return err;
        }
      }

      if (check_keys && !(keys = json_get_array(child, "keys"))) {
        return YANG_SCHEMA_ERROR;
      }
    } else if (type && strcmp(type, "leaf-list") == 0) {
      int next_is_end = i + 1 == end;
      if (!next_is_end) {
        return LIST_NO_FILTER;
      }
    }

    if (keylist) {
      free(keylist);
    }
    if (obj) {
      free(obj);
    }
    iter = child;
  }
  *root_yang = iter;
  return RE_OK;
}

struct json_object *build_recursive(struct json_object *jobj,
                                    struct uci_path *path, error *err,
                                    int root) {
  struct json_object *map = NULL;
  char path_string[512];
  const char *type = NULL;
  if (!(type = json_get_string(jobj, "type"))) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  if (!root) {
    get_path_from_yang(jobj, path);
  }

  if (strcmp(type, "leaf") == 0) {
    return uci_get_leaf(jobj, path, err);
  } else if (strcmp(type, "leaf-list") == 0) {
    struct json_object *retval = uci_get_leaf_list(jobj, path, err);
    path->option = "";
    return retval;
  } else if (strcmp(type, "list") == 0) {
    return uci_get_list(jobj, path, err);
  }

  uci_combine_to_path(path, path_string, sizeof(path_string));
  if (!uci_path_exists(path_string)) {
    *err = NO_SUCH_ELEMENT;
    return NULL;
  }

  struct json_object *top_level = json_object_new_object();
  json_object_object_get_ex(jobj, "map", &map);
  json_object_object_foreach(map, key, val) {
    error err_rec = RE_OK;
    struct json_object *check = build_recursive(val, path, &err_rec, 0);
    if (!check && err_rec != RE_OK && err_rec != UCI_READ_FAILED &&
        err_rec != NO_SUCH_ELEMENT) {
      *err = err_rec;
      return NULL;
    }
    if (err_rec != UCI_READ_FAILED && check) {
      json_object_object_add(top_level, key, check);
    } else if (!check && err_rec == RE_OK && strcmp(type, "container") == 0) {
      json_object_object_add(top_level, key, json_object_new_object());
    }
  }
  *err = RE_OK;
  if (json_object_object_length(top_level) == 0) {
    return NULL;
  }
  return top_level;
}

int data_get(struct cgi_context *cgi, char **pathvec) {
  json_object *module = NULL;
  json_object *top_level = NULL;
  json_object *yang_tree = NULL;
  char *module_name = NULL;
  char *top_level_name = NULL;
  const char *type_string = NULL;
  int retval = 1;
  error err;

  if (split_pair_by_char(pathvec[1], &module_name, &top_level_name, ':')) {
    retval = restconf_badrequest();
    goto done;
  }

  struct uci_path uci = INIT_UCI_PATH();

  module = yang_module_exists(module_name);
  if (!module) {
    retval = restconf_unknown_namespace();
    goto done;
  }
  get_path_from_yang(module, &uci);
  top_level = json_get_object_from_map(module, top_level_name);
  if (!top_level) {
    retval = restconf_badrequest();
    goto done;
  }
  get_path_from_yang(top_level, &uci);
  err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci, 0, 0);
  if (!top_level || err != RE_OK) {
    retval = print_error(err);
    goto done;
  }
  type_string = json_get_string(top_level, "type");
  if (!type_string) {
    retval = restconf_badrequest();
    goto done;
  }
  err = RE_OK;
  yang_tree = build_recursive(top_level, &uci, &err, 1);
  if (!yang_tree && err != RE_OK) {
    retval = print_error(err);
    goto done;
  } else if (!yang_tree && err == RE_OK) {
    yang_tree = json_object_new_object();
  }
  content_type_json();
  headers_end();
  if (strcmp(type_string, "leaf") == 0 ||
      strcmp(type_string, "leaf-list") == 0 ||
      strcmp(type_string, "list") == 0) {
    struct json_object *parent = json_object_new_object();
    char buf[512];
    char *object_requested = pathvec[vector_size(pathvec) - 1];
    char *equal_delimiter = strchr(object_requested, '=');
    int equal_delimiter_index = (size_t)(equal_delimiter - object_requested);
    retval = snprintf(buf, sizeof(buf), "%s:%.*s", module_name,
                      equal_delimiter_index, object_requested);
    if (retval < 0) {
      retval = internal_server_error(cgi);
      goto done;
    }
    json_object_object_add(parent, buf, yang_tree);
    json_pretty_print(parent);
    json_object_put(parent);
  } else if (strcmp(type_string, "container") == 0) {
    struct json_object *parent = json_object_new_object();
    json_object_object_add(parent, pathvec[1], yang_tree);
    json_pretty_print(parent);
    json_object_put(parent);
  }
  retval = 0;
done:
  if (module_name) {
    free(module_name);
  }
  if (top_level_name) {
    free(top_level_name);
  }
  if (yang_tree) {
    json_object_put(yang_tree);
  }
  return retval;
}

int data_post(struct cgi_context *cgi, char **pathvec, int root) {
  json_object *module = NULL;
  json_object *top_level = NULL;
  struct json_object *content = NULL;
  char *module_name = NULL;
  char *top_level_name = NULL;
  char *content_raw = NULL;
  char *root_key_copy = NULL;
  char *root_key = NULL;
  struct json_object *root_object = NULL;
  const char *type_string = NULL;
  int retval = 1;
  int exists;
  error err;
  enum json_tokener_error parse_error;
  char path_string[512];
  char key_out[1024];
  uci_write_pair **cmds = NULL;
  struct uci_path uci = INIT_UCI_PATH();

  if ((content_raw = get_content()) == NULL) {
    retval = restconf_malformed();
    goto done;
  }
  content = json_tokener_parse_verbose(content_raw, &parse_error);
  if (parse_error != json_tokener_success) {
    retval = restconf_malformed();
    goto done;
  }
  if (json_object_object_length(content) != 1) {
    // Only 1 child is allowed
    retval = restconf_malformed();
    goto done;
  }

  json_object_object_foreach(content, root_key_string, root_val) {
    root_key = root_key_string;
    root_object = root_val;
  }

  if (root) {
    if (split_pair_by_char(root_key, &module_name, &top_level_name, ':')) {
      retval = restconf_badrequest();
      goto done;
    }
  } else {
    if (split_pair_by_char(pathvec[1], &module_name, &top_level_name, ':')) {
      retval = restconf_badrequest();
      goto done;
    }
  }

  if (!(module = yang_module_exists(module_name))) {
    retval = restconf_unknown_namespace();
    goto done;
  }

  get_path_from_yang(module, &uci);
  top_level = json_get_object_from_map(module, top_level_name);
  if (!top_level) {
    retval = restconf_badrequest();
    goto done;
  }
  get_path_from_yang(top_level, &uci);

  if (root) {
    content = root_object;
    get_path_from_yang(content, &uci);
  } else {
    err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci, 1, 0);
    if (!top_level || err != RE_OK) {
      retval = print_error(err);
      goto done;
    }
  }

  type_string = json_get_string(top_level, "type");
  if (!type_string || strcmp(type_string, "leaf") == 0) {
    restconf_badrequest();
    goto done;
  }

  uci_combine_to_path(&uci, path_string, sizeof(path_string));
  exists = uci_path_exists(path_string);
  if ((!exists && !root)) {
    retval = restconf_data_missing();
    goto done;
  } else if ((exists && root)) {
    retval = restconf_data_exists();
    goto done;
  }
  if (json_object_get_type(content) != json_type_object) {
    retval = restconf_malformed();
    goto done;
  }

  uci_write_pair *container_create =
      initialize_uci_write_pair(&uci, NULL, container);
  if (!container_create) {
    retval = restconf_operation_failed_internal();
    goto done;
  }
  root_key_copy = str_dup(root_key);
  struct json_object *created = NULL;
  if ((created = json_get_object_from_map(top_level, root_key_copy))) {
    if (strcmp(json_get_string(created, "type"), "list") == 0) {
      struct json_object *keys = NULL;
      if ((keys = json_get_array(created, "keys"))) {
        struct json_object *values = NULL;
        if (json_extract_key_values(keys, root_object, &values) == RE_OK) {
          json_object_object_foreach(values, key, value) {
            strcat(key_out, json_object_get_string(value));
            strcat(key_out, ",");
          }
          key_out[strlen(key_out) - 1] = '\0';
        }
      }
    }
  }
  cmds = verify_content_yang(content, top_level, &uci, &err, !root, 1);
  if (err != RE_OK) {
    retval = print_error(err);
    goto done;
  }
  vector_push_back(cmds, container_create);
  write_uci_write_list(cmds);
  printf("Status: 201 Created\r\n");
  char *protocol = NULL;
  char *slash = "";
  char *equal = "";
  if (strlen(key_out) > 0) {
    equal = "=";
  }
  if (cgi->https) {
    protocol = "https://";
  } else {
    protocol = "http://";
  }
  if (cgi->path_full[strlen(cgi->path_full) - 1] != '/') {
    slash = "/";
  }
  printf("Location: %s%s%s%s%s%s%s\r\n", protocol, cgi->host, cgi->path_full,
         slash, root_key_copy, equal, key_out);
  headers_end();
done:
  if (module_name) {
    free(module_name);
  }
  if (top_level_name) {
    free(top_level_name);
  }
  if (cmds) {
    free_uci_write_list(cmds);
  }
  if (root_key_copy) {
    free(root_key_copy);
  }
  return retval;
}

int data_put(struct cgi_context *cgi, char **pathvec, int root) {
  // Cannot update the key values for list item
  char *content_raw = NULL;
  char *module_name = NULL;
  char *top_level_name = NULL;
  char *root_key = NULL;
  struct json_object *root_object = NULL;
  struct json_object *content = NULL;
  struct json_object *module = NULL;
  json_object *top_level = NULL;
  struct uci_path uci = INIT_UCI_PATH();
  struct uci_path delete_uci = INIT_UCI_PATH();
  enum json_tokener_error parse_error;
  uci_write_pair **cmds = NULL;
  char **package_list = NULL;
  struct uci_path *delete = NULL;
  char key_out[1024];
  error err;
  int retval = 1;

  if ((content_raw = get_content()) == NULL) {
    retval = restconf_malformed();
    goto done;
  }
  content = json_tokener_parse_verbose(content_raw, &parse_error);
  if (parse_error != json_tokener_success) {
    retval = restconf_malformed();
    goto done;
  }
  if (json_object_object_length(content) != 1) {
    // Only 1 child is allowed
    retval = restconf_malformed();
    goto done;
  }

  json_object_object_foreach(content, root_key_string, root_val) {
    root_key = root_key_string;
    root_object = root_val;
  }

  if (root) {
    if (split_pair_by_char(root_key, &module_name, &top_level_name, ':')) {
      retval = restconf_badrequest();
      goto done;
    }
  } else {
    if (split_pair_by_char(pathvec[1], &module_name, &top_level_name, ':')) {
      retval = restconf_badrequest();
      goto done;
    }
  }

  if (!(module = yang_module_exists(module_name))) {
    retval = restconf_unknown_namespace();
    goto done;
  }
  get_path_from_yang(module, &uci);

  top_level = json_get_object_from_map(module, top_level_name);
  if (!top_level) {
    retval = restconf_badrequest();
    goto done;
  }
  get_path_from_yang(top_level, &uci);

  if ((vector_size(pathvec) == 2 || root)) {
    content = root_object;
    get_path_from_yang(content, &uci);
  } else {
    err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci, 1, 1);
    if (!top_level || err != RE_OK) {
      retval = print_error(err);
      goto done;
    }
  }

  if (json_object_get_type(content) != json_type_object) {
    retval = restconf_malformed();
    goto done;
  }

  free(module_name);
  free(top_level_name);
  module_name = NULL;
  top_level_name = NULL;
  split_pair_by_char(root_key, &module_name, &top_level_name, ':');
  if (!top_level_name) {
    top_level_name = root_key;
  }
  char *item = NULL;
  char *keys = NULL;
  char sep = '=';
  if (vector_size(pathvec) == 2 || root) {
    sep = ':';
  }
  split_pair_by_char(pathvec[vector_size(pathvec) - 1], &item, &keys, sep);
  if (!item) {
    item = pathvec[vector_size(pathvec) - 1];
  }
  if ((vector_size(pathvec) == 2 || root) &&
      strcmp(top_level_name, keys) != 0) {
    retval = restconf_badrequest();
    goto done;
  } else if (!(vector_size(pathvec) == 2 || root) &&
             strcmp(top_level_name, item) != 0) {
    retval = restconf_badrequest();
    goto done;
  }

  if (item) free(item);
  if (keys) free(keys);

  delete_uci = uci;

  if (strcmp(json_get_string(top_level, "type"), "list") == 0) {
    struct json_object *keys = NULL;
    if ((keys = json_get_array(top_level, "keys"))) {
      struct json_object *values = NULL;
      if (json_extract_key_values(keys, root_object, &values) == RE_OK) {
        json_object_object_foreach(values, key, value) {
          strcat(key_out, json_object_get_string(value));
          strcat(key_out, ",");
        }
        key_out[strlen(key_out) - 1] = '\0';
      }
    }
  }

  cmds = verify_content_yang(root_object, top_level, &uci, &err,
                             !(vector_size(pathvec) == 2 || root), 0);
  if (err != RE_OK) {
    retval = print_error(err);
    goto done;
  }

  if (strlen(key_out) > 0) {
    char *obj = NULL;
    char *keylist_encoded = NULL;
    char *keylist = NULL;
    split_pair_by_char(pathvec[vector_size(pathvec) - 1], &obj,
                       &keylist_encoded, '=');
    if (obj && keylist_encoded) {
      keylist = malloc(strlen(keylist_encoded) + 1);
      urldecode(keylist, keylist_encoded);
      free(keylist_encoded);
    }
    if (strcmp(keylist, key_out) != 0) {
      free(obj);
      free(keylist);
      retval = restconf_malformed();
      goto done;
    }
    free(obj);
    free(keylist);
  }

  delete = extract_paths(top_level, &delete_uci, &err);
  if (err != RE_OK) {
    retval = print_error(err);
    goto done;
  }

  int created_or_updated = -1;

  for (size_t i = 0; i < vector_size(delete); i++) {
    char path_string[512];
    struct uci_path tmp = delete[i];

    uci_combine_to_path(&tmp, path_string, sizeof(path_string));

    if (!is_in_vector(package_list, tmp.package)) {
      vector_push_back(package_list, tmp.package);
    }

    if ((created_or_updated = uci_delete_path(path_string, false)) == 1) {
      retval = print_error(INTERNAL);
      uci_revert_all(package_list);
      goto done;
    }
  }

  if (uci_commit_all(package_list)) {
    retval = restconf_partial_operation();
    goto done;
  }

  write_uci_write_list(cmds);

  if (created_or_updated == -1) {
    printf("Status: 201 Created\r\n");
    headers_end();
  } else {
    printf("Status: 204 No Content\r\n");
    headers_end();
  }
  retval = 0;
done:
  if (module_name) {
    free(module_name);
  }
  if (top_level_name) {
    free(top_level_name);
  }
  if (cmds) {
    free_uci_write_list(cmds);
  }
  return retval;
}

struct uci_path *extract_paths(struct json_object *node, struct uci_path *uci,
                               error *err) {
  struct json_object *map = NULL;
  struct uci_path *path_list = NULL;

  const char *type = NULL;
  if (!(type = json_get_string(node, "type"))) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }

  if (strcmp(type, "leaf") == 0) {
    // add path of leaf
    vector_push_back(path_list, *uci);
    uci->option = "";
    *err = RE_OK;
    return path_list;
  } else if (strcmp(type, "leaf-list") == 0) {
    // add path of
    vector_push_back(path_list, *uci);
    uci->option = "";
    *err = RE_OK;
    return path_list;
  } else if (strcmp(type, "list") == 0) {
    int list_length = uci_list_length(uci);
    json_object_object_get_ex(node, "map", &map);
    int start = (uci->where) ? uci->index : 0;
    int end = (uci->where) ? uci->index + 1 : list_length;
    for (int index = start; index < end; index++) {
      uci->where = 1;
      uci->index = start;
      vector_push_back(path_list, *uci);
      json_object_object_foreach(map, key, val) {
        const char *subnode_type = NULL;
        if (!(subnode_type = json_get_string(val, "type"))) {
          *err = YANG_SCHEMA_ERROR;
          return NULL;
        }
        if (strcmp(subnode_type, "container") == 0 ||
            strcmp(subnode_type, "list") == 0) {
          struct uci_path *nested_path_list = NULL;
          get_path_from_yang(val, uci);
          nested_path_list = extract_paths(val, uci, err);
          if (*err != RE_OK) {
            return NULL;
          }
          for (size_t i = 0; i < vector_size(nested_path_list); i++) {
            vector_push_back(path_list, nested_path_list[i]);
          }
          vector_free(nested_path_list);
        }
      }
    }
    uci->option = "";
    uci->section_type = "";
    uci->index = 0;
    uci->where = 0;
    *err = RE_OK;
    return path_list;
  }
  vector_push_back(path_list, *uci);
  json_object_object_get_ex(node, "map", &map);
  json_object_object_foreach(map, key, val) {
    const char *subnode_type = NULL;
    if (!(subnode_type = json_get_string(val, "type"))) {
      *err = YANG_SCHEMA_ERROR;
    }
    if (strcmp(subnode_type, "container") == 0 ||
        strcmp(subnode_type, "list") == 0) {
      struct uci_path *nested_path_list = NULL;
      get_path_from_yang(val, uci);
      nested_path_list = extract_paths(val, uci, err);
      if (*err != RE_OK) {
        return NULL;
      }
      for (size_t i = 0; i < vector_size(nested_path_list); i++) {
        vector_push_back(path_list, nested_path_list[i]);
      }
      vector_free(nested_path_list);
    }
  }
  *err = RE_OK;
  return path_list;
}

int data_delete(struct cgi_context *cgi, char **pathvec, int root) {
  json_object *module = NULL;
  json_object *top_level = NULL;
  char *module_name = NULL;
  char *top_level_name = NULL;
  int retval = 1;
  struct uci_path uci = INIT_UCI_PATH();
  struct uci_path *delete = NULL;
  char **package_list = NULL;
  error err;
  char exists_path[512];

  if (split_pair_by_char(pathvec[1], &module_name, &top_level_name, ':')) {
    retval = restconf_badrequest();
    goto done;
  }

  if (!(module = yang_module_exists(module_name))) {
    retval = restconf_unknown_namespace();
    goto done;
  }
  get_path_from_yang(module, &uci);

  top_level = json_get_object_from_map(module, top_level_name);
  if (!top_level) {
    retval = restconf_badrequest();
    goto done;
  }
  get_path_from_yang(top_level, &uci);

  err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci, 1, 0);
  if (!top_level || err != RE_OK) {
    retval = print_error(err);
    goto done;
  }
  uci_combine_to_path(&uci, exists_path, sizeof(exists_path));
  if (!uci_path_exists(exists_path)) {
    retval = restconf_invalid_value();
    goto done;
  }
  if (yang_mandatory(top_level)) {
    retval = restconf_badrequest();
    goto done;
  }

  delete = extract_paths(top_level, &uci, &err);
  if (err != RE_OK) {
    retval = print_error(err);
    goto done;
  }
  for (size_t i = 0; i < vector_size(delete); i++) {
    char path_string[512];
    struct uci_path tmp = delete[i];

    uci_combine_to_path(&tmp, path_string, sizeof(path_string));

    if (!is_in_vector(package_list, tmp.package)) {
      vector_push_back(package_list, tmp.package);
    }

    if (uci_delete_path(path_string, false) == 1) {
      retval = print_error(INTERNAL);
      uci_revert_all(package_list);
      goto done;
    }
  }
  if (uci_commit_all(package_list)) {
    retval = restconf_partial_operation();
    goto done;
  }
  retval = 0;
  printf("Status: 204 No Content\r\n");
  headers_end();
done:
  if (delete) {
    vector_free(delete);
  }
  if (package_list) {
    vector_free(package_list);
  }
  return retval;
}