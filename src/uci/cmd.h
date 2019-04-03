#ifndef RESTCONF_CMD_H
#define RESTCONF_CMD_H

#include "methods.h"

enum uci_object_type { list, option, container };

struct uci_write_pair {
  struct uci_path path;
  char *value;
  enum uci_object_type type;
};
typedef struct uci_write_pair uci_write_pair;

struct uci_write_pair *initialize_uci_write_pair(struct uci_path *path,
                                                 char *value,
                                                 enum uci_object_type type);
int write_uci_write_list(uci_write_pair **write_list);
int free_uci_write_list(uci_write_pair **list);

#endif  // RESTCONF_CMD_H
