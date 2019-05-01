#ifndef RESTCONF_ERROR_H
#define RESTCONF_ERROR_H

enum error {
  RE_OK,
  INTERNAL,
  NO_SUCH_ELEMENT,
  LIST_NO_FILTER,
  LIST_UNDEFINED_KEY,
  LEAF_NO_OPTION,
  YANG_SCHEMA_ERROR,
  UCI_READ_FAILED,
  INVALID_TYPE,
  ELEMENT_ALREADY_EXISTS,
  KEY_NOT_PRESENT,
  IDENTICAL_KEYS,
  MANDATORY_NOT_PRESENT,
  MULTIPLE_OBJECTS,
  DELETING_KEY
};
typedef enum error error;

int restconf_badrequest();
int restconf_malformed();
int restconf_data_exists();
int restconf_data_missing();
int restconf_invalid_value();
int restconf_unknown_namespace();
int restconf_missing_element();
int restconf_partial_operation();
int restconf_operation_failed();
int restconf_operation_failed_internal();
int restconf_unknown_element();

int print_error(error err);

#endif  // RESTCONF_ERROR_H
