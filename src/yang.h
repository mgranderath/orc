#ifndef _YANG_H
#define _YANG_H
#include <stddef.h>

struct map_str2str{
  char *key;
  char *str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
  {"restconf-example", "{'type': 'module', 'uci': 'restconf', 'map': {'state': {'type': 'container', 'uci': 'state', 'map': {'op': {'type': 'leaf-list', 'uci': 'op', 'map': {}}}}}}"}
};
#endif