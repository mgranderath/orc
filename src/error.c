#include "error.h"
#include <json.h>
#include <stdio.h>
#include "http.h"
#include "restconf-json.h"

int restconf_error(char *error_tag) {
  struct json_object *jobj = json_object_new_object();
  struct json_object *ietf = json_object_new_object();
  struct json_object *array = json_object_new_array();

  struct json_object *error_message = json_object_new_object();
  json_object_object_add(error_message, "error-type",
                         json_object_new_string("protocol"));
  json_object_object_add(error_message, "error-tag",
                         json_object_new_string(error_tag));
  json_object_array_add(array, error_message);
  json_object_object_add(ietf, "error", array);
  json_object_object_add(jobj, "ietf-restconf:errors", ietf);

  json_pretty_print(jobj);
  json_object_put(error_message);
  json_object_put(array);
  json_object_put(ietf);
  json_object_put(jobj);
  return 0;
}

int restconf_badrequest() {
  printf("Status: 400 Bad Request\r\n");
  content_type_json();
  headers_end();
  restconf_error("unknown-element");
  return 0;
}

int restconf_malformed() {
  printf("Status: 400 Bad Request\r\n");
  content_type_json();
  headers_end();
  restconf_error("malformed-message");
  return 0;
}

int restconf_data_exists() {
  printf("Status: 409 Conflict\r\n");
  content_type_json();
  headers_end();
  restconf_error("data-exists");
  return 0;
}

int restconf_missing_element() {
  printf("Status: 409 Conflict\r\n");
  content_type_json();
  headers_end();
  restconf_error("missing-element");
  return 0;
}

int restconf_data_missing() {
  printf("Status: 409 Conflict\r\n");
  content_type_json();
  headers_end();
  restconf_error("data-missing");
  return 0;
}

int restconf_invalid_value() {
  printf("Status: 404 Not Found\r\n");
  content_type_json();
  headers_end();
  restconf_error("invalid-value");
  return 0;
}

int restconf_unknown_namespace() {
  printf("Status: 400 Bad Request\r\n");
  content_type_json();
  headers_end();
  restconf_error("unknown-namespace");
  return 0;
}

int restconf_partial_operation() {
  printf("Status: 500 Internal Server Error\r\n");
  content_type_json();
  headers_end();
  restconf_error("partial-operation");
  return 0;
}

int restconf_operation_failed() {
  printf("Status: 412 Precondition Failed\r\n");
  content_type_json();
  headers_end();
  restconf_error("operation-failed");
  return 0;
}

int restconf_operation_failed_internal() {
  printf("Status: 500 Internal Server Error\r\n");
  content_type_json();
  headers_end();
  restconf_error("operation-failed");
  return 0;
}

int restconf_unknown_element() {
  printf("Status: 400 Bad Request\r\n");
  content_type_json();
  headers_end();
  restconf_error("unknown-element");
  return 0;
}

int print_error(error err) {
  switch (err) {
    case LEAF_NO_OPTION:
    case YANG_SCHEMA_ERROR:
    case INTERNAL:
      restconf_operation_failed_internal();
      break;
    case NO_SUCH_ELEMENT:
      restconf_invalid_value();
      break;
    case LIST_UNDEFINED_KEY:
      restconf_operation_failed();
      break;
    case INVALID_TYPE:
      restconf_malformed();
      break;
    case MANDATORY_NOT_PRESENT:
    case KEY_NOT_PRESENT:
      restconf_missing_element();
      break;
    case ELEMENT_ALREADY_EXISTS:
    case IDENTICAL_KEYS:
      restconf_data_exists();
      break;
    case LIST_NO_FILTER:
    case MULTIPLE_OBJECTS:
      restconf_badrequest();
      break;
    case DELETING_KEY:
      restconf_badrequest();
    default:
      break;
  }
  return 0;
}