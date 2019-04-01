#include "restconf-method.h"
#include <json_object_private.h>
#include "error.h"
#include "http.h"
#include "restconf-json.h"
#include "url.h"
#include "vector.h"

int free_cmd_list(uci_write_pair **list) {
  for (size_t i = 0; i < vector_size(list); i++) {
    uci_write_pair *cmd = list[i];
    free(cmd->value);
  }
  vector_free(list);
  return 0;
}

int get_path_from_yang(struct json_object *jobj, struct uci_path *uci) {
  struct json_object *uci_value = NULL;
  json_object_object_get_ex(jobj, "uci-package", &uci_value);
  if (uci_value) {
    if (json_object_get_type(uci_value) == json_type_string) {
      uci->package = (char *)json_object_get_string(uci_value);
    }
  }
  json_object_object_get_ex(jobj, "uci-section", &uci_value);
  if (uci_value) {
    if (json_object_get_type(uci_value) == json_type_string) {
      uci->section = (char *)json_object_get_string(uci_value);
    }
  }
  json_object_object_get_ex(jobj, "uci-option", &uci_value);
  if (uci_value) {
    if (json_object_get_type(uci_value) == json_type_string) {
      uci->option = (char *)json_object_get_string(uci_value);
    }
  }
  json_object_object_get_ex(jobj, "uci-section-type", &uci_value);
  if (uci_value) {
    if (json_object_get_type(uci_value) == json_type_string) {
      uci->section_type = (char *)json_object_get_string(uci_value);
      uci->section = "";
      uci->option = "";
      uci->index = 0;
      uci->where = 0;
    }
  }
  return 0;
}

uci_write_pair **verify_content_yang(struct json_object *content,
                                     struct json_object *yang_node,
                                     struct uci_path *path, error *err,
                                     int root) {
  uci_write_pair **command_list = NULL;
  if (root) {
    get_path_from_yang(yang_node, path);
  }
  json_object_object_foreach(content, key, val) {
    struct json_object *child = NULL;
    const char *child_type = NULL;
    const char *child_leaf_type_string = NULL;
    yang_type child_leaf_type = NONE;
    char path_string[512];
    json_type value_type = json_object_get_type(val);
    child = json_get_object_from_map(yang_node, key);
    if (!child) {
      *err = NO_SUCH_ELEMENT;
      return NULL;
    }
    get_path_from_yang(child, path);
    child_type = json_get_string(child, "type");
    if (!child_type) {
      *err = YANG_SCHEMA_ERROR;
      return NULL;
    }
    if (strcmp(child_type, "leaf") == 0) {
      if (value_type == json_type_object || value_type == json_type_array) {
        *err = INVALID_TYPE;
        return NULL;
      }
      child_leaf_type_string = json_get_string(child, "leaf-type");
      child_leaf_type = str_to_yang_type(child_leaf_type_string);
      if (verify_yang_json_type(child_leaf_type, value_type)) {
        *err = INVALID_TYPE;
        return NULL;
      }
      if (path->where) {
        combine_to_anonymous_path(path, path->index, path_string,
                                  sizeof(path_string));
      } else {
        combine_to_path(path, path_string, sizeof(path_string));
      }
      int exists = uci_path_exists(path_string);
      if (exists) {
        *err = ELEMENT_ALREADY_EXISTS;
        return NULL;
      }
      const char *value = json_object_get_string(val);
      uci_write_pair *output = malloc(sizeof(uci_write_pair));
      output->path = *path;
      output->value = (char *)value;
      output->type = option;
      vector_push_back(command_list, output);
    } else if (strcmp(child_type, "leaf-list") == 0) {
      if (value_type != json_type_array) {
        *err = INVALID_TYPE;
        return NULL;
      }
      for (int i = 0; i < json_object_array_length(val); i++) {
        json_type element_type =
            json_object_get_type(json_object_array_get_idx(val, i));
        if (verify_yang_json_type(child_leaf_type, element_type)) {
          *err = INVALID_TYPE;
          return NULL;
        }
      }
      if (path->where) {
        combine_to_anonymous_path(path, path->index, path_string,
                                  sizeof(path_string));
      } else {
        combine_to_path(path, path_string, sizeof(path_string));
      }
      int exists = uci_path_exists(path_string);
      if (exists) {
        *err = ELEMENT_ALREADY_EXISTS;
        return NULL;
      }
      for (int i = 0; i < json_object_array_length(val); i++) {
        json_object *item = json_object_array_get_idx(val, i);
        const char *value = json_object_get_string(item);
        uci_write_pair *output = malloc(sizeof(uci_write_pair));
        output->path = *path;
        output->value = (char *)value;
        output->type = list;
        vector_push_back(command_list, output);
      }
    } else if (strcmp(child_type, "list") == 0) {
      if (value_type != json_type_array) {
        *err = INVALID_TYPE;
        return NULL;
      }
      // TODO: Actual type checking implementation
      int list_length = uci_list_length(path);
      if (list_length != 0) {
        *err = ELEMENT_ALREADY_EXISTS;
        return NULL;
      }
      *err = json_yang_verify_list(val, child);
      if (*err != RE_OK) {
        return NULL;
      }
      int array_length = json_object_array_length(val);
      for (int index = 0; index < array_length; index++) {
        struct json_object *tmp = json_object_array_get_idx(val, index);
        path->index = index;
        path->where = 1;
        uci_write_pair **tmp_list =
            verify_content_yang(tmp, child, path, err, 0);
        if (*err != RE_OK) {
          free_cmd_list(tmp_list);
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
        free_cmd_list(tmp_list);
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
    return NO_PATH;
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
    if (obj) {
      path_computed = obj;
    } else {
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
    } else if (next_is_end) {
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
      printf("not as many keys as keys specified\n");
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

struct json_object *get_list(struct json_object *jobj, struct uci_path *path,
                             error *err) {
  struct json_object *jarray = json_object_new_array();
  const char *type = NULL;

  type = json_get_string(jobj, "type");
  if (!type) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  get_path_from_yang(jobj, path);
  if (strlen(path->section_type) == 0 || strlen(path->section) != 0) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  path->index = 0;
  path->where = 1;
  while (1) {
    struct json_object *top_level = json_object_new_object();
    struct json_object *map = NULL;
    int notfound = 0;
    json_object_object_get_ex(jobj, "map", &map);
    json_object_object_foreach(map, key, val) {
      error err_rec = RE_OK;
      struct json_object *check = build_recursive(val, path, &err_rec);
      if (!check) {
        notfound = 1;
        break;
      }
      json_object_object_add(top_level, key, check);
    }
    if (notfound) {
      break;
    }
    json_object_array_add(jarray, top_level);
    path->index++;
  }
  *err = RE_OK;
  path->index = 0;
  path->where = 0;
  return jarray;
}

struct json_object *get_leaf(struct json_object *jobj, struct uci_path *path,
                             error *err) {
  char path_string[512], buf[512];
  int failed;
  const char *leaf_type_string = NULL;
  yang_type leaf_type;
  struct json_object *output = NULL;

  if (path->where) {
    combine_to_anonymous_path(path, path->index, path_string,
                              sizeof(path_string));
  } else {
    combine_to_path(path, path_string, sizeof(path_string));
  }
  failed = uci_read_option(path_string, buf, sizeof(buf));
  if (failed) {
    *err = UCI_READ_FAILED;
    return NULL;
  }
  leaf_type_string = json_get_string(jobj, "leaf-type");
  if (!leaf_type_string) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  leaf_type = str_to_yang_type(leaf_type_string);
  if (leaf_type == NONE) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  output = json_yang_type_format(leaf_type, buf);
  if (!output) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  *err = RE_OK;
  return output;
}

struct json_object *get_leaf_list(struct json_object *jobj,
                                  struct uci_path *path, error *err) {
  char path_string[512];
  char **items = NULL;
  const char *leaf_type_string;
  yang_type leaf_type;
  struct json_object *output = json_object_new_array();

  if (path->where) {
    combine_to_anonymous_path(path, path->index, path_string,
                              sizeof(path_string));
  } else {
    combine_to_path(path, path_string, sizeof(path_string));
  }
  // initialize the vector so we can pass i
  items = uci_read_list(path_string);
  if (!items) {
    *err = UCI_READ_FAILED;
    return NULL;
  }
  leaf_type_string = json_get_string(jobj, "leaf-type");
  if (!leaf_type_string) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  leaf_type = str_to_yang_type(leaf_type_string);
  if (leaf_type == NONE) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  for (size_t i = 0; i < vector_size(items); i++) {
    struct json_object *iter = NULL;
    iter = json_yang_type_format(leaf_type, items[i]);
    if (!iter) {
      *err = YANG_SCHEMA_ERROR;
      return NULL;
    }
    json_object_array_add(output, iter);
  }
  vector_free(items);
  *err = RE_OK;
  return output;
}

struct json_object *build_recursive(struct json_object *jobj,
                                    struct uci_path *path, error *err) {
  struct json_object *map = NULL, *uci_value = NULL;
  const char *type = json_get_string(jobj, "type");
  if (!type) {
    *err = YANG_SCHEMA_ERROR;
    return NULL;
  }
  get_path_from_yang(jobj, path);

  if (strcmp(type, "leaf") == 0) {
    return get_leaf(jobj, path, err);
  } else if (strcmp(type, "leaf-list") == 0) {
    struct json_object *leaf_list = get_leaf_list(jobj, path, err);
    if (*err == UCI_READ_FAILED) {
      *err = RE_OK;
      return json_object_new_array();
    }
    return leaf_list;
  } else if (strcmp(type, "list") == 0) {
    return get_list(jobj, path, err);
  }
  struct json_object *top_level = json_object_new_object();
  json_object_object_get_ex(jobj, "map", &map);
  json_object_object_foreach(map, key, val) {
    error err_rec = RE_OK;
    struct json_object *check = build_recursive(val, path, &err_rec);
    if (!check && err_rec != RE_OK && err_rec != UCI_READ_FAILED) {
      *err = err_rec;
      return NULL;
    }
    if (err_rec != UCI_READ_FAILED && check) {
      json_object_object_add(top_level, key, check);
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
  char *delimiter = NULL;
  const char *type_string = NULL;
  int retval = 1;
  size_t delimiter_index;
  error err;

  delimiter = strchr(pathvec[1], ':');
  if (!delimiter) {
    // No : found in top
    retval = restconf_badrequest();
    goto done;
  }
  delimiter_index = (size_t)(delimiter - pathvec[1]);
  module_name = strn_dup(pathvec[1], delimiter_index);
  top_level_name = str_dup(pathvec[1] + delimiter_index + 1);
  if (!module_name || !top_level_name) {
    // copying the individual components failed
    retval = restconf_badrequest(cgi);
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
    retval = restconf_badrequest(cgi);
    goto done;
  }
  get_path_from_yang(top_level, &uci);
  err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci);
  if (!top_level || err != RE_OK) {
    switch (err) {
      case LIST_NO_FILTER:
      case LIST_UNDEFINED_KEY:
        retval = restconf_invalid_value();
        break;
      default:
        retval = restconf_badrequest();
    }
    goto done;
  }
  type_string = json_get_string(top_level, "type");
  if (!type_string) {
    restconf_badrequest(cgi);
    goto done;
  }
  err = RE_OK;
  yang_tree = build_recursive(top_level, &uci, &err);
  if (!yang_tree || err != RE_OK) {
    restconf_badrequest(cgi);
    goto done;
  }
  content_type_json();
  headers_end();
  if (strcmp(type_string, "leaf") == 0 ||
      strcmp(type_string, "leaf-list") == 0 ||
      strcmp(type_string, "list") == 0) {
    struct json_object *parent = json_object_new_object(),
                       *leaf = json_object_new_object();
    char buf[512];
    retval = snprintf(buf, sizeof(buf), "%s:%s", module_name,
                      pathvec[vector_size(pathvec) - 1]);
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
  return retval;
}

int section_already_created(struct path_section_pair *sections,
                            char *section_type, int index) {
  for (size_t i = 0; i < vector_size(sections); i++) {
    struct path_section_pair curr = sections[i];
    if (strcmp(curr.section_type, section_type) == 0 && curr.index == index) {
      return 1;
    }
  }
  return 0;
}

int split_module_top_level(char *split, char **module, char **top_level) {
  char *delimiter = NULL;
  size_t delimiter_index;
  delimiter = strchr(split, ':');
  if (!delimiter) {
    // No : found in top
    return 1;
  }
  delimiter_index = (size_t)(delimiter - split);
  *module = strn_dup(split, delimiter_index);
  *top_level = str_dup(split + delimiter_index + 1);
  if (!module || !top_level) {
    return 1;
  }
  return 0;
}

int data_post(struct cgi_context *cgi, char **pathvec, int root) {
  json_object *module = NULL;
  json_object *top_level = NULL;
  struct json_object *content = NULL;
  char *module_name = NULL;
  char *top_level_name = NULL;
  char *content_raw = NULL;
  char *root_key = NULL;
  struct json_object *root_object;
  const char *type_string = NULL;
  int retval = 1;
  int exists;
  error err;
  enum json_tokener_error parse_error;
  char path_string[512];
  json_type root_type;
  uci_write_pair **cmds = NULL;
  struct uci_path uci = INIT_UCI_PATH();

  if ((content_raw = get_content()) == NULL) {
    retval = restconf_malformed(cgi);
    goto done;
  }
  content = json_tokener_parse_verbose(content_raw, &parse_error);
  if (parse_error != json_tokener_success) {
    retval = restconf_malformed(cgi);
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

  if (!root) {
    int split_failed =
        split_module_top_level(pathvec[1], &module_name, &top_level_name);
    if (split_failed) {
      retval = restconf_badrequest();
      goto done;
    }

    if ((module = yang_module_exists(module_name)) == NULL) {
      retval = restconf_unknown_namespace();
      goto done;
    }
    get_path_from_yang(module, &uci);
    if ((top_level = json_get_object_from_map(module, top_level_name)) ==
        NULL) {
      retval = restconf_badrequest(cgi);
      goto done;
    }
    get_path_from_yang(top_level, &uci);
    err = check_path(&top_level, pathvec, 2, vector_size(pathvec), &uci);
    if (!top_level || err != RE_OK) {
      switch (err) {
        case LIST_NO_FILTER:
        case LIST_UNDEFINED_KEY:
          retval = restconf_invalid_value();
          break;
        default:
          retval = restconf_badrequest();
      }
      goto done;
    }
    type_string = json_get_string(top_level, "type");
    if (!type_string || strcmp(type_string, "leaf") == 0) {
      restconf_badrequest(cgi);
      goto done;
    }
  } else {
    if (split_module_top_level(root_key, &module_name, &top_level_name)) {
      retval = restconf_badrequest();
      goto done;
    }

    module = yang_module_exists(module_name);
    if (!module) {
      retval = restconf_unknown_namespace();
      goto done;
    }

    get_path_from_yang(module, &uci);
    top_level = json_get_object_from_map(module, top_level_name);
    if (!top_level) {
      retval = restconf_badrequest(cgi);
      goto done;
    }
    get_path_from_yang(top_level, &uci);
    type_string = json_get_string(top_level, "type");
    if (!type_string || strcmp(type_string, "leaf") == 0) {
      restconf_badrequest(cgi);
      goto done;
    }
    content = root_object;
  }

  if (uci.where) {
    combine_to_anonymous_path(&uci, uci.index, path_string,
                              sizeof(path_string));
  } else {
    combine_to_path(&uci, path_string, sizeof(path_string));
  }
  exists = uci_path_exists(path_string);
  if ((!exists && !root) || (exists && root)) {
    retval = restconf_data_missing(cgi);
    goto done;
  }
  // TODO: What to return for list?
  root_type = json_object_get_type(content);
  if (strcmp(type_string, "container") == 0 && root_type != json_type_object) {
    retval = restconf_malformed();
    goto done;
  } else if (strcmp(type_string, "container") != 0 &&
             root_type != json_type_array) {
    retval = restconf_malformed();
    goto done;
  }
  uci_write_pair *container_create = malloc(sizeof(uci_write_pair));
  container_create->path = uci;
  container_create->value = NULL;
  container_create->type = container;
  cmds = verify_content_yang(content, top_level, &uci, &err, 1);
  if (err != RE_OK) {
    retval = restconf_malformed();
    goto done;
  }
  vector_push_back(cmds, container_create);
  struct path_section_pair *section_list = NULL;
  for (size_t i = 0; i < vector_size(cmds); i++) {
    int failed = 1;
    char local_path_string[512];
    uci_write_pair *cmd = cmds[i];
    if (cmd->path.where && cmd->path.section_type) {
      int section_exists = section_already_created(
          section_list, cmd->path.section_type, cmd->path.index);
      if (!section_exists) {
        uci_add_section_anon(cmd->path.package, cmd->path.section_type);
        struct path_section_pair output = {
            .section_type = cmd->path.section_type, .index = cmd->path.index};
        vector_push_back(section_list, output);
      }
      combine_to_anonymous_path(&cmd->path, cmd->path.index, local_path_string,
                                sizeof(local_path_string));
    } else {
      uci_add_section_named(cmd->path.package, "container", cmd->path.section);
      combine_to_path(&cmd->path, local_path_string, sizeof(local_path_string));
    }
    switch (cmd->type) {
      case list:
        failed = uci_write_list(local_path_string, cmd->value);
        break;
      case option:
        failed = uci_write_option(local_path_string, cmd->value);
        break;
      case container:
        failed = uci_add_section_named(cmd->path.package, "container",
                                       cmd->path.section);
        break;
    }
    if (failed) {
      // TODO: Discard delta
    }
  }
  printf("Status: 201 Created\r\n");
  char *protocol = NULL;
  if (cgi->https) {
    protocol = "https://";
  } else {
    protocol = "http://";
  }
  printf("Location: %s%s%s%s\r\n", protocol, cgi->host, cgi->path_full,
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
    free_cmd_list(cmds);
  }
  return retval;
}
