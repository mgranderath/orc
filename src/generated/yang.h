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
     "{\"type\": \"module\", \"map\": {\"state\": {\"type\": \"container\", "
     "\"map\": {\"op\": {\"type\": \"leaf-list\", \"map\": {}, \"uci-option\": "
     "\"op\", \"leaf-type\": \"int64\"}, \"num\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"numa\", \"leaf-type\": \"int8\"}, "
     "\"people\": {\"type\": \"list\", \"map\": {\"name\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"name\", \"leaf-type\": "
     "\"string\"}, \"age\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"age\", \"leaf-type\": \"int8\"}, \"age2\": {\"type\": \"leaf\", "
     "\"map\": {}, \"mandatory\": true, \"uci-option\": \"age2\", "
     "\"leaf-type\": \"int8\"}, \"age-list\": {\"type\": \"leaf-list\", "
     "\"map\": {}, \"uci-option\": \"age_list\", \"leaf-type\": \"int64\"}}, "
     "\"uci-section-type\": \"people_list\", \"keys\": [\"name\", \"age\"], "
     "\"mandatory\": [\"age2\"]}, \"nested-state\": {\"type\": \"container\", "
     "\"map\": {\"top\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"top\", \"leaf-type\": \"string\"}}, \"uci-section\": \"nested_state\", "
     "\"uci-section-type\": \"nested_state\"}}, \"uci-section\": \"state\", "
     "\"uci-section-type\": \"state\"}}, \"uci-package\": \"restconf\"}"},
    {"openwrt-system",
     "{\"type\": \"module\", \"map\": {\"system\": {\"type\": \"container\", "
     "\"map\": {\"hostname\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"hostname\", \"leaf-type\": \"string\"}, "
     "\"buffersize\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"buffersize\", \"leaf-type\": \"uint32\"}, \"conloglevel\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"conloglevel\", \"leaf-type\": "
     "{\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"7\"}}, "
     "\"cronloglevel\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"cronloglevel\", \"leaf-type\": \"uint8\"}, \"klogconloglevel\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"klogconloglevel\", "
     "\"leaf-type\": \"uint8\"}, \"log_buffer_size\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"lof_buffer_size\", \"leaf-type\": "
     "\"uint32\"}, \"log_file\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_file\", \"leaf-type\": \"string\"}, "
     "\"log_hostname\": {\"type\": \"leaf\", \"map\": {}, \"leaf-type\": "
     "\"string\"}, \"log_ip\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_ip\", \"leaf-type\": \"inet:ipv4-address\"}, "
     "\"log_port\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"log_port\", \"leaf-type\": \"uint32\"}, \"log_prefix\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"log_prefix\", \"leaf-type\": "
     "\"string\"}, \"log_proto\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_proto\", \"leaf-type\": {\"leaf-type\": "
     "\"string\", \"pattern\": \"^(udp|tcp)$\"}}, \"log_remote\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"log_remote\", \"leaf-type\": "
     "\"boolean\"}, \"log_size\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_size\", \"leaf-type\": \"uint32\"}, "
     "\"log_trailer_null\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"log_trailer_null\", \"leaf-type\": \"boolean\"}, \"log_type\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"log_type\", "
     "\"leaf-type\": {\"leaf-type\": \"string\", \"pattern\": "
     "\"^(circular|file)$\"}}, \"urandom_seed\": {\"type\": \"leaf\", \"map\": "
     "{}, \"uci-option\": \"urandom_seed\", \"leaf-type\": \"string\"}, "
     "\"timezone\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"timezone\", \"leaf-type\": \"string\"}, \"zonename\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"zonename\", \"leaf-type\": "
     "\"string\"}}, \"uci-package\": \"system\", \"uci-section\": \"OpenWrt\", "
     "\"uci-section-type\": \"system\"}}}"}};

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

static const map_str2str yang2regex[] = {
    {"inet:ipv4-address",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "\"^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\\\.){3}([0-9]|[1-"
     "9]["
     "0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])(%[\\\\p{N}\\\\p{L}]+)?$\"}"}};

struct json_object* yang_module_exists(char* module);
yang_type str_to_yang_type(const char* str);
const char* yang_for_type(char* type);
#endif