#include "restconf-json.h"
#include <json.h>
#include <stdio.h>

const char* json_get_string(struct json_object *jobj, const char* key) {
    struct json_object *tmp = NULL;
    json_object_object_get_ex(jobj, key, &tmp);
    if (!tmp || json_object_get_type(tmp) != json_type_string) {
        return NULL;
    }
    return json_object_get_string(tmp);
}

struct json_object* json_get_object_from_map(struct json_object *jobj, const char *key) {
    json_object *tmp = NULL;
    json_object_object_get_ex(jobj, "map", &tmp);
    json_object_object_get_ex(tmp, key, &tmp);
    return tmp;
}

void json_pretty_print(struct json_object* jobj) {
    printf("%s\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}

yang_type str_to_yang_type(const char* str) {
    const map_str2yang_type *iter = str2yang_type;
    const map_str2yang_type *end = str2yang_type + sizeof(str2yang_type) / sizeof(str2yang_type[0]);
    while (iter && iter < end) {
        if (strlen(str) == strlen(iter->str) && strcmp(str, iter->str) == 0) {
            return iter->type;
        }
        iter++;
    }
    return NONE;
}

struct json_object* json_yang_type_format(yang_type type, const char* val) {
    if (type == INT_8 || type == INT_16 || type == INT_32 || type == UINT_8 || type == UINT_16 || type == UINT_32) {
        int32_t integer = strtoimax(val, NULL, 10);
        return json_object_new_int(integer);
    }
    if (type == INT_64 || type == UINT_64) {
        int64_t integer = strtoimax(val, NULL, 10);
        return json_object_new_int64(integer);
    }
    if (type == STRING) {
        return json_object_new_string(val);
    }
    return NULL;
}

