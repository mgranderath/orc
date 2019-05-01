#include "restconf-verify.h"
#include <uci/uci-util.h>
#include "yang-verify.h"

/**
 * @brief verify a leaf list according to existence etc
 * @param value the leaf-list JSON array
 * @param path the UCI path pointing at that item
 * @param yang the YANG node for the leaf-list
 * @param check_exists check if node already exists
 * @return RE_OK if success else error
 */
error restconf_verify_leaf_list(struct json_object *value,
                                struct uci_path *path, struct json_object *yang,
                                int check_exists) {
  error err;
  if ((err = yang_verify_leaf_list(value, yang)) != RE_OK) {
    return err;
  }
  if (check_exists) {
    if (uci_element_exists(path)) {
      return ELEMENT_ALREADY_EXISTS;
    }
  }
  return RE_OK;
}

/**
 * @brief verify a leaf according to existence etc
 * @param value the leaf JSON object
 * @param path the UCI path pointing at that item
 * @param yang the YANG node for the leaf-
 * @param check_exists check if node already exists
 * @return RE_OK if success else error
 */
error restconf_verify_leaf(struct json_object *value, struct uci_path *path,
                           struct json_object *yang, int check_exists) {
  error err;
  if ((err = yang_verify_leaf(value, yang)) != RE_OK) {
    return err;
  }
  if (check_exists) {
    if (uci_element_exists(path)) {
      return ELEMENT_ALREADY_EXISTS;
    }
  }
  return RE_OK;
}