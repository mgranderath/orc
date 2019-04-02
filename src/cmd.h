#ifndef RESTCONF_CMD_H
#define RESTCONF_CMD_H

#include "restconf-uci.h"

struct uci_write_pair {
  struct uci_path path;
  char *value;
  enum { list, option, container } type;
};
typedef struct uci_write_pair uci_write_pair;

int write_uci_write_list(uci_write_pair **write_list);
int free_uci_write_list(uci_write_pair **list);

#endif  // RESTCONF_CMD_H
