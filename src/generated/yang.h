#ifndef _YANG_H
#define _YANG_H

#include <stddef.h>
#include <string.h>
#include <json.h>

struct map_str2str {
    char *key;
    char *str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
        {"restconf-example", "{'type': 'module', 'uci-package': 'restconf', 'map': {'state': {'type': 'container', 'uci-section': 'state',"
                             " 'map': {'op': {'type': 'leaf-list', 'leaf-type': 'int64', 'uci-option': 'op', 'map': {}}, 'num': {'type': 'leaf', 'leaf-type': 'int8', 'uci-option': 'numa', 'map': {}}}}}}"}
};

enum yang_type {
    NONE,
    BINARY,
    BITS,
    BOOLEAN,
    DECIMAL_64,
    EMPTY,
    ENUMERATION,
    IDENTITY_REF,
    INSTANCE_IDENTIFIER,
    INT_8,
    INT_16,
    INT_32,
    INT_64,
    LEAF_REF,
    STRING,
    UINT_8,
    UINT_16,
    UINT_32,
    UINT_64,
    UNION
};
typedef enum yang_type yang_type;

struct map_str2yang_type {
    char* str;
    yang_type type;
};
typedef struct map_str2yang_type map_str2yang_type;

static const map_str2yang_type str2yang_type[] = {
        {"binary", BINARY},
        {"bits", BITS},
        {"boolean", BOOLEAN},
        {"decimal64", DECIMAL_64},
        {"emtpy", EMPTY},
        {"enumeration", ENUMERATION},
        {"identityref", IDENTITY_REF},
        {"instance-identifier", INSTANCE_IDENTIFIER},
        {"int8", INT_8},
        {"int16", INT_16},
        {"int32", INT_32},
        {"int64", INT_64},
        {"leafref", LEAF_REF},
        {"string", STRING},
        {"uint8", UINT_8},
        {"uint16", UINT_16},
        {"uint32", UINT_32},
        {"uint64", UINT_64},
        {"union", UNION},
};
#endif