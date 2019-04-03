#include "uci/uci-util.h"
#include "cmd.h"
#include "error.h"
#include "http.h"
#include "restconf-json.h"
#include "restconf-method.h"
#include "uci-util.h"
#include "uci/uci-get.h"
#include "url.h"
#include "util.h"
#include "vector.h"

/**
 * Combines the uci_path struct to a string
 * @param path the uci_path* to be combined
 * @param buffer char* into which the output should be copied
 * @param size the size of the buffer
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int combine_to_path(struct uci_path *path, char *buffer, size_t size) {
  int retval = -1;
  if (!path->option || strlen(path->option) == 0) {
    retval = snprintf(buffer, size, "%s.%s", path->package, path->section);
  } else {
    retval = snprintf(buffer, size, "%s.%s.%s", path->package, path->section,
                      path->option);
  }
  if (retval < 1) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * Combines the uci_path struct and index into an anonymous section path
 * @param path the uci_path* to be combined
 * @param index the index of the anonymous section
 * @param buffer char* into which the output should be copied
 * @param size the size of the buffer
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int combine_to_anonymous_path(struct uci_path *path, int index, char *buffer,
                              size_t size) {
  int retval = -1;
  if (!path->option || strlen(path->option) == 0) {
    retval = snprintf(buffer, size, "%s.@%s[%d]", path->package,
                      path->section_type, index);
  } else {
    retval = snprintf(buffer, size, "%s.@%s[%d].%s", path->package,
                      path->section_type, index, path->option);
  }
  if (retval < 1) {
    return 1;
  } else {
    return 0;
  }
}

int uci_combine_to_path(struct uci_path *path, char *buffer,
                        size_t buffer_size) {
  if (path->where) {
    combine_to_anonymous_path(path, path->index, buffer, buffer_size);
  } else {
    combine_to_path(path, buffer, buffer_size);
  }
  return 0;
}

int get_path_from_yang(struct json_object *jobj, struct uci_path *uci) {
  struct json_object *uci_value = NULL;
  char *uci_package = NULL;
  char *uci_section = NULL;
  char *uci_option = NULL;
  json_object_object_get_ex(jobj, "uci-package", &uci_value);
  if (json_object_get_type(uci_value) == json_type_string) {
    uci_package = (char *)json_object_get_string(uci_value);
  }
  json_object_object_get_ex(jobj, "uci-section", &uci_value);
  if (json_object_get_type(uci_value) == json_type_string) {
    uci_section = (char *)json_object_get_string(uci_value);
  }
  json_object_object_get_ex(jobj, "uci-option", &uci_value);
  if (json_object_get_type(uci_value) == json_type_string) {
    uci_option = (char *)json_object_get_string(uci_value);
  }

  if (uci_package) {
    if (uci_section) {
      if (uci_option) {
        uci->package = uci_package;
        uci->section = uci_section;
        uci->option = uci_option;
        return 0;
      }
      uci->package = uci_package;
      uci->section = uci_section;
      uci->option = "";
      return 0;
    }
    uci->package = uci_package;
    uci->section = "";
    uci->option = "";
    return 0;
  }

  if (uci_section) {
    if (uci_option) {
      uci->section = uci_section;
      uci->option = uci_option;
      return 0;
    }
    uci->section = uci_section;
    uci->option = "";
    return 0;
  }

  if (uci_option) {
    uci->option = uci_option;
    return 0;
  }

  json_object_object_get_ex(jobj, "uci-section-type", &uci_value);
  if (uci_value) {
    if (json_object_get_type(uci_value) == json_type_string) {
      uci->section_type = (char *)json_object_get_string(uci_value);
      uci->section = "";
      uci->option = "";
      uci->index = 0;
      uci->where = 0;
    }
  }
  return 0;
}