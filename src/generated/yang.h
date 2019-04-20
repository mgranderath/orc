#ifndef _YANG_H
#define _YANG_H
#include <json.h>
#include <stddef.h>
#include <string.h>

struct map_str2str {
  char* key;
  char* str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
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
     "\"uci-section-type\": \"system\"}}}"},
    {"restconf-example",
     "{\"type\": \"module\", \"map\": {\"course\": {\"type\": \"container\", "
     "\"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"name\", \"leaf-type\": \"string\"}, \"semester\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"semester\", \"leaf-type\": "
     "{\"leaf-type\": \"uint8\", \"from\": \"1\", \"to\": \"6\"}}, "
     "\"instructors\": {\"type\": \"leaf-list\", \"map\": {}, \"uci-option\": "
     "\"instructors\", \"leaf-type\": \"string\"}, \"students\": {\"type\": "
     "\"list\", \"map\": {\"firstname\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"firstname\", \"leaf-type\": \"string\"}, \"lastname\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"lastname\", "
     "\"leaf-type\": \"string\"}, \"age\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"age\", \"leaf-type\": {\"leaf-type\": \"uint8\", "
     "\"from\": \"0\", \"to\": \"120\"}}, \"major\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"major\", \"leaf-type\": {\"leaf-type\": "
     "\"string\", \"pattern\": \"^(CS|IMS)$\"}}, \"grade\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"grade\", \"leaf-type\": "
     "\"grade\"}}, \"uci-section-type\": \"student\", \"keys\": "
     "[\"firstname\", \"lastname\", \"age\"]}, \"instructor\": {\"type\": "
     "\"container\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"name\", \"leaf-type\": \"string\"}, \"email\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"email\", "
     "\"leaf-type\": \"email\"}}, \"uci-section\": \"instructor\", "
     "\"uci-section-type\": \"instructor\"}}, \"uci-section\": \"course\", "
     "\"uci-section-type\": \"course\"}}, \"uci-package\": "
     "\"restconf-example\"}"}};

static const map_str2str yang2regex[] = {
    {"grade", "{\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"100\"}"},
    {"email",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "\"^[A-Za-z0-9]*@university.de$\"}"},
    {"inet:ipv4-address",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "\"^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\\\.){3}([0-9]|[1-"
     "9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])(%[\\\\p{N}\\\\p{L}]+)?$\"}"}};

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
    {"union", UNION}};

struct json_object* yang_module_exists(char* module);
yang_type str_to_yang_type(const char* str);
const char* yang_for_type(const char* type);

#endif
