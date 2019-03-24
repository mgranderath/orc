#include <json.h>
#include <stddef.h>
#include <stdio.h>
#include "restconf-method.h"
#include "cgi.h"
#include "http.h"
#include "vector.h"
#include "yang.h"
#include "restconf-uci.h"
#include "restconf-json.h"
#include <uci.h>

struct json_object *value_in_modules(char *module) {
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

struct json_object *check_path(struct json_object *jobj, char **path, size_t start, size_t end, struct uci_path *uci) {
    struct json_object *iter = jobj;
    if (path) {
        size_t i;
        for (i = start; i < end; i++) {
            struct json_object *tmp = json_get_object_from_map(iter, path[i]);
            if (!tmp) {
                return NULL;
            } else {
                struct json_object *uci_value = NULL;
                json_object_object_get_ex(tmp, "uci-option", &uci_value);
                if (uci_value) {
                    if (json_object_get_type(uci_value) == json_type_string) {
                        uci->option = (char *) json_object_get_string(uci_value);
                    }
                }
                iter = tmp;
            }
        }
    }
    return iter;
}

int restconf_error(struct cgi_context *ctx) {
    struct json_object *jobj = json_object_new_object();
    struct json_object *ietf = json_object_new_object();
    struct json_object *array = json_object_new_array();

    struct json_object *error_message = json_object_new_object();
    json_object_object_add(error_message, "error-type", json_object_new_string("protocol"));
    json_object_object_add(error_message, "error-tag", json_object_new_string("unknown-element"));
    json_object_array_add(array, error_message);
    json_object_object_add(ietf, "error", array);
    json_object_object_add(jobj, "ietf-restconf:errors", ietf);

    json_pretty_print(jobj);
    json_object_put(error_message);
    json_object_put(array);
    json_object_put(ietf);
    json_object_put(jobj);
    return 0;
}

int restconf_badrequest(struct cgi_context *ctx) {
    printf("Status: 400 Bad Request\r\n");
    content_type_json();
    headers_end();
    restconf_error(ctx);
    return 0;
}

struct json_object* build_recursive(struct json_object *jobj, struct uci_path *path) {
    struct json_object *map = NULL, *top_level = json_object_new_object();
    const char* type = json_get_string(jobj, "type");
    if (!type) {
        return NULL;
    }
    if (strcmp(type, "leaf") == 0) {
        content_type_json();
        headers_end();
        char buf[256];
        uci_read_option(path, buf);
        const char* type_leaf = json_get_string(top_level, "leaf-type");
        yang_type leaf_type = str_to_yang_type(type_leaf);
        struct json_object *value = json_yang_type_format(leaf_type, buf);
        if (!value) {
            return NULL;
        }
        json_object_object_add(top_level, path->option, value);
        return top_level;
    } else if (strcmp(type, "leaf-list") == 0) {
        content_type_json();
        headers_end();
        struct json_object *jarray = json_object_new_array();
        const char* type_leaf = json_get_string(top_level, "leaf-type");
        yang_type leaf_type = str_to_yang_type(type_leaf);
        char **items = NULL;
        vector_grow(items, 1);
        uci_read_list(path, items);
        for (size_t i = 0; i < vector_size(items); i++) {
            struct json_object *value = json_yang_type_format(leaf_type, items[i]);
            if (!value) {
                return NULL;
            }
            json_object_array_add(jarray, value);
        }
        json_object_object_add(top_level, path->option, jarray);
        vector_free(items);
        return top_level;
    }
    json_object_object_get_ex(jobj, "map", &map);
    json_object_object_foreach(map, key, val) {
        json_object_object_add(top_level, key, build_recursive(val, path));
    }
    return top_level;
}

int data_get(struct cgi_context *cgi, char **pathvec) {
    json_object *module, *top_level;
    char *pathvec_modify = NULL;
    char **vec = NULL;
    int retval = 1;

    pathvec_modify = str_dup(pathvec[1]);
    vec = path2vec(pathvec_modify, ":");
    if (vec == NULL) {
        goto done;
    }
    if (vector_size(vec) < 2) {
        retval = badrequest(cgi);
        goto done;
    }
    struct uci_path uci = {
            "",
            "",
            ""
    };

    module = value_in_modules(vec[0]);
    struct json_object *uci_value = NULL;
    json_object_object_get_ex(module, "uci-package", &uci_value);
    if (uci_value) {
        if (json_object_get_type(uci_value) == json_type_string) {
            uci.package = (char *) json_object_get_string(uci_value);
        }
    }
    if (!module) {
        restconf_badrequest(cgi);
        goto done;
    }
    top_level = json_get_object_from_map(module, vec[1]);
    uci_value = NULL;
    json_object_object_get_ex(top_level, "uci-section", &uci_value);
    if (uci_value) {
        if (json_object_get_type(uci_value) == json_type_string) {
            uci.section = (char *) json_object_get_string(uci_value);
        }
    }
    if (!top_level) {
        restconf_badrequest(cgi);
        goto done;
    }
    top_level = check_path(top_level, pathvec, 2, vector_size(pathvec), &uci);
    if (!top_level) {
        restconf_badrequest(cgi);
        goto done;
    }
    const char* type = json_get_string(top_level, "type");
    if (!type) {
        restconf_badrequest(cgi);
        goto done;
    }
    if (strcmp(type, "leaf") == 0) {
        content_type_json();
        headers_end();
        char buf[256];
        uci_read_option(&uci, buf);
        const char* type_leaf = json_get_string(top_level, "leaf-type");
        yang_type leaf_type = str_to_yang_type(type_leaf);
        struct json_object *value = json_yang_type_format(leaf_type, buf);
        if (!value) {
            retval = internal_server_error(cgi);
            goto done;
        }
        struct json_object *jobj = json_object_new_object();
        struct json_object *parent = json_object_new_object();
        json_object_object_add(jobj, pathvec[vector_size(pathvec) - 1], value);
        json_object_object_add(parent, pathvec[1], jobj);
        json_pretty_print(parent);
        json_object_put(parent);
    } else if (strcmp(type, "leaf-list") == 0) {
        content_type_json();
        headers_end();
        struct json_object *jobj = json_object_new_object();
        struct json_object *jarray = json_object_new_array();
        struct json_object *parent = json_object_new_object();
        const char* type_leaf = json_get_string(top_level, "leaf-type");
        yang_type leaf_type = str_to_yang_type(type_leaf);
        char **items = NULL;
        vector_grow(items, 1);
        retval = uci_read_list(&uci, items);
        if (retval) {
            goto done;
        }
        for (size_t i = 0; i < vector_size(items); i++) {
            struct json_object *value = json_yang_type_format(leaf_type, items[i]);
            if (!value) {
                retval = internal_server_error(cgi);
                goto done;
            }
            json_object_array_add(jarray, value);
        }
        json_object_object_add(jobj, pathvec[vector_size(pathvec) - 1], jarray);
        json_object_object_add(parent, pathvec[1], jobj);
        json_pretty_print(parent);
        vector_free(items);
        json_object_put(parent);
    } else if (strcmp(type, "container") == 0) {
        content_type_json();
        headers_end();
    }
    retval = 0;
    done:
    if (vec) {
        vector_free(vec);
    }
    if (pathvec_modify) {
        free(pathvec_modify);
    }
    return retval;
}