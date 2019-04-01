#ifndef RESTCONF_ERROR_H
#define RESTCONF_ERROR_H

enum error {
  RE_OK,
  INTERNAL,
  NO_SUCH_ELEMENT,
  LIST_NO_FILTER,
  LIST_UNDEFINED_KEY,
  LEAF_NO_OPTION,
  NO_PATH,
  YANG_SCHEMA_ERROR,
  UCI_READ_FAILED,
  INVALID_TYPE,
  ELEMENT_ALREADY_EXISTS,
  KEY_NOT_PRESENT,
  IDENTICAL_KEYS
};
typedef enum error error;

int restconf_error(char *error_tag);

int restconf_badrequest();
int restconf_malformed();
int restconf_data_exists();
int restconf_data_missing();
int restconf_invalid_value();
int restconf_unknown_namespace();

#endif  // RESTCONF_ERROR_H
