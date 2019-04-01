#ifndef _YANG_H
#define _YANG_H

#include <json.h>
#include <stddef.h>
#include <string.h>

struct map_str2str {
  const char* key;
  const char* str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
    {"restconf-example",
     "{\"type\": \"module\",\"uci-package\": \"restconf\",\"map\": {\"state\": "
     "{\"type\": \"container\",\"uci-section\": \"state\",\"map\": {\"op\": "
     "{\"type\": \"leaf-list\",\"leaf-type\": \"int64\",\"uci-option\": "
     "\"op\",\"map\": {}},\"num\": {\"type\": \"leaf\",\"leaf-type\": "
     "\"int8\",\"uci-option\": \"numa\",\"map\": {}},\"people\": {\"type\": "
     "\"list\",\"uci-section-type\": \"people_list\",\"keys\": [\"name\", "
     "\"age\"],\"map\": {\"name\": {\"type\": \"leaf\",\"leaf-type\": "
     "\"string\",\"uci-option\": \"name\",\"map\": {}},\"age\": {\"type\": "
     "\"leaf\",\"leaf-type\": \"int8\",\"uci-option\": \"age\",\"map\": "
     "{}},\"age-list\": {\"type\": \"leaf-list\",\"leaf-type\": "
     "\"int64\",\"uci-option\": \"age_list\",\"map\": {}}}},\"nested-state\": "
     "{\"type\": \"container\",\"uci-section\": \"nested_state\",\"map\": "
     "{\"top\": {\"type\": \"leaf\",\"leaf-type\": \"string\",\"uci-option\": "
     "\"top\",\"map\": {}}}}}}}}"}};

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

struct json_object* yang_module_exists(char* module);
yang_type str_to_yang_type(const char* str);
#endif