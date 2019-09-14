#ifndef RESTCONF_CMD_H
#define RESTCONF_CMD_H

#include "methods.h"

enum uci_object_type { list, option, container };

struct UciWritePair {
  struct UciPath path;
  char *value;
  enum uci_object_type type;
};
typedef struct UciWritePair UciWritePair;

struct UciWritePair *initialize_uci_write_pair(struct UciPath *path,
                                               char *value,
                                               enum uci_object_type type);
int write_uci_write_list(UciWritePair **write_list);
int free_uci_write_list(UciWritePair **list);

#endif  // RESTCONF_CMD_H
