#ifndef _RESTCONF_UCI_H
#define _RESTCONF_UCI_H

#include <stddef.h>
#include <uci.h>

struct uci_path {
    char* package;
    char* section;
    char* option;
};

int combine_to_path(struct uci_path *path, char *buffer, size_t size);
int uci_read_option(struct uci_path *path, char *buffer);
int uci_read_list(struct uci_path *path, char **ret);

#endif //_YANG_UCI_H
