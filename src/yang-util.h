//
// Created by Malte Granderath on 03/09/2019.
//

#ifndef RESTCONF_YANG_UTIL_H
#define RESTCONF_YANG_UTIL_H

#define YANG_UCI_OPTION "option"
#define YANG_UCI_PACKAGE "package"
#define YANG_UCI_SECTION_NAME "section-name"
#define YANG_UCI_SECTION "section"
#define YANG_UCI_LEAF_AS_NAME "leaf-as-name"
#define YANG_KEYS "keys"
#define YANG_UNIQUE "unique"
#define YANG_MANDATORY "mandatory"
#define YANG_LEAF_TYPE "leaf-type"
#define YANG_MAP "map"
#define YANG_TYPE "type"
#define YANG_LEAF "leaf"
#define YANG_LEAF_LIST "leaf-list"
#define YANG_CONTAINER "container"
#define YANG_LIST "list"

int yang_is_leaf(const char* type_string);
int yang_is_list(const char* type_string);
int yang_is_leaf_list(const char* type_string);
int yang_is_container(const char* type_string);

#endif  // RESTCONF_YANG_UTIL_H
