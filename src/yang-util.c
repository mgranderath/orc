//
// Created by Malte Granderath on 03/09/2019.
//

#include "yang-util.h"
#include <string.h>

int yang_is_leaf(const char* type_string) {
  return strcmp(type_string, YANG_LEAF) == 0;
}

int yang_is_list(const char* type_string) {
  return strcmp(type_string, YANG_LIST) == 0;
}

int yang_is_leaf_list(const char* type_string) {
  return strcmp(type_string, YANG_LEAF_LIST) == 0;
}

int yang_is_container(const char* type_string) {
  return strcmp(type_string, YANG_CONTAINER) == 0;
}