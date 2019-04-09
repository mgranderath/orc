#include "restconf-verify.h"
#include <uci/uci-util.h>
#include "yang-verify.h"

error restconf_verify_leaf_list(struct json_object *value,
                                struct uci_path *path, struct json_object *yang,
                                int check_exists) {
  error err;
  if ((err = yang_verify_leaf_list(value, yang)) != RE_OK) {
    return err;
  }
  if (check_exists) {
    if (yang_element_exists(path)) {
      return ELEMENT_ALREADY_EXISTS;
    }
  }
  return RE_OK;
}

error restconf_verify_leaf(struct json_object *value, struct uci_path *path,
                           struct json_object *yang, int check_exists) {
  error err;
  if ((err = yang_verify_leaf(value, yang)) != RE_OK) {
    return err;
  }
  if (check_exists) {
    if (yang_element_exists(path)) {
      return ELEMENT_ALREADY_EXISTS;
    }
  }
  return RE_OK;
}