
#ifndef _YANG_H
#define _YANG_H
#include <stddef.h>
#include <json-c/json.h>
#include <string.h>

struct map_str2str{
  char *key;
  char *str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
    {"restconf-example", "{\"type\": \"module\", \"map\": {\"course\": {\"type\": \"container\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"string\"}, \"semester\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"semester\", \"leaf-type\": {\"leaf-type\": \"uint8\", \"from\": \"1\", \"to\": \"6\"}}, \"instructors\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"instructors\", \"leaf-type\": \"string\"}, \"students\": {\"type\": \"list\", \"map\": {\"firstname\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"firstname\", \"leaf-type\": \"string\"}, \"lastname\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"lastname\", \"leaf-type\": \"string\"}, \"age\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"age\", \"leaf-type\": {\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"120\"}}, \"major\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"major\", \"leaf-type\": {\"leaf-type\": \"string\", \"pattern\": \"^(CS|IMS)$\"}}, \"grade\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"grade\", \"leaf-type\": \"grade\"}}, \"section\": \"student\", \"leaf-as-name\": \"lastname\", \"keys\": [\"firstname\", \"lastname\", \"age\"]}, \"instructor\": {\"type\": \"container\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"string\"}, \"email\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"email\", \"leaf-type\": \"email\"}}, \"section-name\": \"instructor\", \"section\": \"instructor\"}}, \"section-name\": \"course\", \"section\": \"course\"}}, \"package\": \"restconf-example\"}"}
};

static const map_str2str yang2regex[] = {
    {"grade", "{\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"100\"}"},
    {"email", "{\"leaf-type\": \"string\", \"pattern\": \"^[A-Za-z0-9]*@university.de$\"}"}
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
    {"string", STRING},
    {"int8", INT_8},
    {"int16", INT_16},
    {"int32", INT_32},
    {"int64", INT_64},
    {"uint8", UINT_8},
    {"uint16", UINT_16},
    {"uint32", UINT_32},
    {"uint64", UINT_64},
    {"binary", BINARY},
    {"boolean", BOOLEAN},
    {"decimal64", DECIMAL_64}
};

struct json_object* yang_module_exists(char* module);
yang_type str_to_yang_type(const char* str);
const char* yang_for_type(const char* type);

#endif