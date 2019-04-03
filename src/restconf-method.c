#include "restconf-method.h"
#include "error.h"
#include "http.h"
#include "restconf-json.h"
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
                                     int root) {
  uci_write_pair **command_list = NULL;
  json_object_object_foreach(content, key, val) {
    struct json_object *child = NULL;
    const char *child_type = NULL;
    char *module = NULL;
    char *split_key = NULL;
    char path_string[512];
    if (split_module_top_level(key, &module, &split_key)) {
      split_key = key;
    }
    if (module) {
      free(module);
    }
    json_type value_type = json_object_get_type(val);
    child = json_get_object_from_map(yang_node, split_key);
    free(split_key);
    if (!child) {
      *err = NO_SUCH_ELEMENT;
      return NULL;
    }
    get_path_from_yang(child, path);
    if (!(child_type = json_get_string(child, "type"))) {
      *err = YANG_SCHEMA_ERROR;
      return NULL;
    }
    if (strcmp(child_type, "leaf") == 0) {
      if ((*err = yang_verify_leaf(val, child)) != RE_OK) {
        return NULL;
      }
      uci_combine_to_path(path, path_string, sizeof(path_string));
      if (uci_path_exists(path_string)) {
        *err = ELEMENT_ALREADY_EXISTS;
        return NULL;
      }
      uci_write_pair *output = initialize_uci_write_pair(
          path, (char *)json_object_get_string(val), option);
      if (!output) {
        *err = INTERNAL;
        return NULL;
      }
      vector_push_back(command_list, output);
    } else if (strcmp(child_type, "leaf-list") == 0) {
      if ((*err = yang_verify_leaf_list(val, child)) != RE_OK) {
        return NULL;
      }
      uci_combine_to_path(path, path_string, sizeof(path_string));
      if (uci_path_exists(path_string)) {
        *err = ELEMENT_ALREADY_EXISTS;
        return NULL;
      }
      json_array_forloop(val, index) {
        json_object *item = json_object_array_get_idx(val, index);
        const char *value = json_object_get_string(item);
        uci_write_pair *output =
            initialize_uci_write_pair(path, (char *)value, list);
        if (!output) {
          *err = INTERNAL;
          return NULL;
        }
        vector_push_back(command_list, output);
      }
    } else if (strcmp(child_type, "list") == 0) {
      if (value_type != json_type_array && value_type != json_type_object) {
        *err = INVALID_TYPE;
        return NULL;
      }
      // TODO: Actual type checking implementation
      int list_length = uci_list_length(path);
      if (root) {
        if (value_type != json_type_object) {
          *err = MULTIPLE_OBJECTS;
          return NULL;
        }
        struct json_object *existing_values = uci_get_list(child, path, err);
        if (*err != RE_OK) {
          return NULL;
        }
        // combine with uci values when root is list
        json_object_array_add(existing_values, val);
        *err = json_yang_verify_list(existing_values, child);
        if (*err != RE_OK) {
          return NULL;
        }
      } else {
        if (list_length != 0) {
          *err = ELEMENT_ALREADY_EXISTS;
          return NULL;
        }

        *err = json_yang_verify_list(val, child);
        if (*err != RE_OK) {
          return NULL;
        }
      }
      if (value_type == json_type_array) {
        // TODO: check if only one array item can be added at a time
        json_array_forloop(val, index) {
          struct json_object *tmp = json_object_array_get_idx(val, index);
          path->index = index;
          path->where = 1;
          uci_write_pair **tmp_list =
              verify_content_yang(tmp, child, path, err, 0);
          if (*err != RE_OK) {
            free_uci_write_list(tmp_list);
            return NULL;
          }
          for (size_t i = 0; i < vector_size(tmp_list); i++) {
            vector_push_back(command_list, tmp_list[i]);
          }
          // TODO: Solve memory leak problem with tmp list
          vector_free(tmp_list);
        }
      } else {
        path->index = list_length;
        path->where = 1;
        uci_write_pair **tmp_list =
            verify_content_yang(val, child, path, err, 0);
        if (*err != RE_OK) {
          free_uci_write_list(tmp_list);
          return NULL;
        }
        for (size_t i = 0; i < vector_size(tmp_list); i++) {
          vector_push_back(command_list, tmp_list[i]);
        }
        // TODO: Solve memory leak problem with tmp list
        vector_free(tmp_list);
      }
      path->index = 0;
      path->where = 0;
    } else if (strcmp(child_type, "container") == 0) {
      if (value_type != json_type_object) {
        return NULL;
      }
      // TODO: Actual type checking implementation
      uci_write_pair **tmp_list = verify_content_yang(val, child, path, err, 0);
      if (*err != RE_OK) {
        free_uci_write_list(tmp_list);
        return NULL;
      }
      for (size_t i = 0; i < vector_size(tmp_list); i++) {
        vector_push_back(command_list, tmp_list[i]);
      }
      // TODO: Solve memory leak problem with tmp list
      vector_free(tmp_list);
    }
  }
  *err = RE_OK;
  return command_list;
}

error check_path(struct json_object **jobj, char **path, size_t start,
                 size_t end, struct uci_path *uci) {
  struct json_object *iter = *jobj;
  size_t i;
  if (!path) {
    return INTERNAL;
  }
  for (i = start; i < end; i++) {
    char *path_computed = NULL;
    char *obj = NULL;
    char *keylist = NULL;
    char *keylist_encoded = NULL;
    char *delimiter = NULL;
    const char *type = NULL;
    struct json_object *child = NULL;
    struct json_object *keys = NULL;
    int array_length;

    delimiter = strchr(path[i], '=');
    if (delimiter) {
      size_t index = (size_t)(delimiter - path[i]);
      obj = strn_dup(path[i], index);
      keylist_encoded = str_dup(path[i] + index + 1);
      keylist = malloc(strlen(keylist_encoded) + 1);
      urldecode(keylist, keylist_encoded);
      free(keylist_encoded);
      if (!obj || !keylist) {
        // copying the individual components failed
        return INTERNAL;
      }
    }

    if (!(path_computed = obj)) {
      path_computed = path[i];
    }

    child = json_get_object_from_map(iter, path_computed);
    if (!child) {
      return NO_SUCH_ELEMENT;
    }
    get_path_from_yang(child, uci);
    iter = child;
    type = json_get_string(iter, "type");
    if (type && strcmp(type, "list") != 0) {
      if (keylist) {
        free(keylist);
      }
      if (obj) {
        free(obj);
      }
      continue;
    }
    int next_is_end = i + 1 == end;
    if (!delimiter && !next_is_end) {
      return LIST_NO_FILTER;
    } else if (!delimiter && next_is_end) {
      break;
    }

    keys = json_get_array(iter, "keys");
    if (!keys) {
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
      key_child = json_get_object_from_map(iter, yang_option_name);
      if (!key_child) {
        return LEAF_NO_OPTION;
      }
      uci_option_name = json_get_string(key_child, "uci-option");
      if (uci_option_name == NULL) {
        return LEAF_NO_OPTION;
      }
      key_value[index].key = uci_option_name;
      key_value[index].str = keylist_vec[index];
    }
    struct uci_where where = {
        .path = uci, .key_value = key_value, .key_value_length = array_length};
    int where_index = uci_index_where(&where);
    if (where_index == -1) {
      return LIST_UNDEFINED_KEY;
    }

    uci->where = 1;
    uci->index = where_index;
    vector_free(keylist_vec);
    if (keylist) {
      free(keylist);
    }
    if (obj) {
      free(obj);
    }
  }
  *jobj = iter;
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
    return uci_get_leaf_list(jobj, path, err);
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

  if (split_module_top_level(pathvec[1], &module_name, &top_level_name)) {
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
  err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci);
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
  char *root_key = NULL;
  struct json_object *root_object = NULL;
  const char *type_string = NULL;
  int retval = 1;
  int exists;
  error err;
  enum json_tokener_error parse_error;
  char path_string[512];
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
    if (split_module_top_level(root_key, &module_name, &top_level_name)) {
      retval = restconf_badrequest();
      goto done;
    }
  } else {
    if (split_module_top_level(pathvec[1], &module_name, &top_level_name)) {
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
    err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci);
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
  if ((!exists && !root) || (exists && root)) {
    retval = restconf_data_exists();
    goto done;
  }
  // TODO: What to return for list?
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

  cmds = verify_content_yang(content, top_level, &uci, &err, !root);
  if (err != RE_OK) {
    retval = print_error(err);
    goto done;
  }
  vector_push_back(cmds, container_create);
  write_uci_write_list(cmds);
  printf("Status: 201 Created\r\n");
  char *protocol = NULL;
  char *slash = "";
  if (cgi->https) {
    protocol = "https://";
  } else {
    protocol = "http://";
  }
  if (cgi->path_full[strlen(cgi->path_full) - 1] != '/') {
    slash = "/";
  }
  printf("Location: %s%s%s%s%s\r\n", protocol, cgi->host, cgi->path_full, slash,
         root_key);
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
  return retval;
}
