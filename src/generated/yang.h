
#ifndef _YANG_H
#define _YANG_H
#include <stddef.h>
#include <json-c/json.h>
#include <string.h>

struct map_str2str{
  char *key;
  char *str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
    {"restconf-example", "{\"type\": \"module\", \"map\": {\"course\": {\"type\": \"container\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"string\"}, \"semester\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"semester\", \"leaf-type\": {\"leaf-type\": \"uint8\", \"from\": \"1\", \"to\": \"6\"}}, \"instructors\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"instructors\", \"leaf-type\": \"string\"}, \"students\": {\"type\": \"list\", \"map\": {\"firstname\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"firstname\", \"leaf-type\": \"string\"}, \"lastname\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"lastname\", \"leaf-type\": \"string\"}, \"age\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"age\", \"leaf-type\": {\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"120\"}}, \"major\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"major\", \"leaf-type\": {\"leaf-type\": \"string\", \"pattern\": \"^(CS|IMS)$\"}}, \"grade\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"grade\", \"leaf-type\": \"grade\"}, \"contact-option\": {\"type\": \"list\", \"map\": {\"email\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"email\", \"leaf-type\": \"string\"}, \"example-leaf\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"example-leaf\", \"leaf-type\": \"string\"}}, \"section\": \"contact_option\", \"leaf-as-name\": \"email\", \"keys\": [\"email\"]}, \"option\": {\"type\": \"list\", \"map\": {\"id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"id\", \"leaf-type\": \"string\"}, \"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"string\"}, \"more-options\": {\"type\": \"list\", \"map\": {\"id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"id\", \"leaf-type\": \"string\"}, \"value\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"value\", \"leaf-type\": \"string\"}}, \"section\": \"more_options\", \"leaf-as-name\": \"id\", \"keys\": [\"id\"]}}, \"section\": \"options\", \"leaf-as-name\": \"id\", \"keys\": [\"id\"]}}, \"section\": \"student\", \"leaf-as-name\": \"lastname\", \"keys\": [\"firstname\", \"lastname\", \"age\"]}, \"instructor\": {\"type\": \"container\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"string\"}, \"email\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"email\", \"leaf-type\": \"email\"}}, \"section-name\": \"instructor\", \"section\": \"instructor\"}}, \"section-name\": \"course\", \"section\": \"course\"}}, \"package\": \"restconf-example\"}"},
    {"lmapd", "{\"type\": \"module\", \"map\": {\"lmap\": {\"type\": \"container\", \"map\": {\"capabilities\": {\"type\": \"container\", \"map\": {\"version\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"version\", \"leaf-type\": \"string\", \"mandatory\": true}, \"tag\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"tag\", \"leaf-type\": \"lmap:tag\"}, \"tasks\": {\"type\": \"container\", \"map\": {\"task\": {\"type\": \"list\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"lmap:identifier\"}, \"version\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"version\", \"leaf-type\": \"string\"}, \"program\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"program\", \"leaf-type\": \"string\"}, \"function\": {\"type\": \"list\", \"map\": {\"uri\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"uri\", \"leaf-type\": \"inet:uri\"}, \"role\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"role\", \"leaf-type\": \"string\"}}, \"section\": \"function\", \"leaf-as-name\": \"uri\", \"keys\": [\"uri\"]}}, \"section\": \"task\", \"leaf-as-name\": \"name\", \"keys\": [\"name\"]}}, \"section\": \"tasks\", \"section-name\": \"tasks\"}}, \"section\": \"capabilities\", \"section-name\": \"capabilities\", \"mandatory\": [\"version\"]}, \"agent\": {\"type\": \"container\", \"map\": {\"agent-id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"agent_id\", \"leaf-type\": \"yang:uuid\"}, \"group-id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"group_id\", \"leaf-type\": \"string\"}, \"measurement-point\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"measurement_point\", \"leaf-type\": \"string\"}, \"report-agent-id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"report_agent_id\", \"leaf-type\": \"boolean\"}, \"report-group-id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"report_group_id\", \"leaf-type\": \"boolean\"}, \"report-measurement-point\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"report_measurement_point\", \"leaf-type\": \"boolean\"}, \"controller-timeout\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"controller_timeout\", \"leaf-type\": \"uint32\"}, \"last-started\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_started\", \"leaf-type\": \"yang:date-and-time\", \"mandatory\": true}}, \"section\": \"agent\", \"section-name\": \"agent\", \"mandatory\": [\"last-started\"]}, \"tasks\": {\"type\": \"container\", \"map\": {\"task\": {\"type\": \"list\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"lmap:identifier\"}, \"program\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"program\", \"leaf-type\": \"string\"}, \"function\": {\"type\": \"list\", \"map\": {\"uri\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"uri\", \"leaf-type\": \"inet:uri\"}, \"role\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"role\", \"leaf-type\": \"string\"}}, \"section\": \"function\", \"leaf-as-name\": \"uri\", \"keys\": [\"uri\"]}, \"option\": {\"type\": \"list\", \"map\": {\"id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"id\", \"leaf-type\": \"lmap:identifier\"}, \"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"string\"}, \"value\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"value\", \"leaf-type\": \"string\"}}, \"section\": \"task_option\", \"leaf-as-name\": \"id\", \"keys\": [\"id\"]}, \"tag\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"tag\", \"leaf-type\": \"lmap:identifier\"}}, \"section\": \"task\", \"leaf-as-name\": \"name\", \"keys\": [\"name\"]}}, \"section\": \"tasks\", \"section-name\": \"tasks\"}, \"schedules\": {\"type\": \"container\", \"map\": {\"schedule\": {\"type\": \"list\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"lmap:identifier\"}, \"start\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"start\", \"leaf-type\": \"event-ref\", \"mandatory\": true}, \"end\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"end\", \"leaf-type\": \"event-ref\"}, \"duration\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"duration\", \"leaf-type\": \"uint32\"}, \"execution-mode\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"execution_mode\", \"leaf-type\": \"enumeration\"}, \"state\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"state\", \"leaf-type\": \"enumeration\", \"mandatory\": true}, \"storage\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"storage\", \"leaf-type\": \"yang:gauge64\", \"mandatory\": true}, \"invocations\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"invocations\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"suppressions\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"suppressions\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"overlaps\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"overlaps\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"failures\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"failures\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"last-invocation\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_invocation\", \"leaf-type\": \"yang:date-and-time\"}, \"tag\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"tag\", \"leaf-type\": \"lmap:tag\"}, \"suppression-tag\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"suppression_tag\", \"leaf-type\": \"lmap:tag\"}, \"action\": {\"type\": \"list\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"lmap:identifier\"}, \"task\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"task\", \"leaf-type\": \"task-ref\", \"mandatory\": true}, \"state\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"state\", \"leaf-type\": \"enumeration\", \"mandatory\": true}, \"storage\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"storage\", \"leaf-type\": \"yang:gauge64\", \"mandatory\": true}, \"invocations\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"invocations\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"suppressions\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"suppressions\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"overlaps\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"overlaps\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"failures\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"failures\", \"leaf-type\": \"yang:counter32\", \"mandatory\": true}, \"last-invocation\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_invocation\", \"leaf-type\": \"yang:date-and-time\", \"mandatory\": true}, \"last-completion\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_completion\", \"leaf-type\": \"yang:date-and-time\", \"mandatory\": true}, \"last-status\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_status\", \"leaf-type\": \"lmap:status-code\", \"mandatory\": true}, \"last-message\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_message\", \"leaf-type\": \"string\", \"mandatory\": true}, \"last-failed-completion\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_failed_completion\", \"leaf-type\": \"yang:date-and-time\", \"mandatory\": true}, \"last-failed-status\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_failed_status\", \"leaf-type\": \"lmap:status-code\", \"mandatory\": true}, \"last-failed-message\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"last_failed_message\", \"leaf-type\": \"string\", \"mandatory\": true}, \"option\": {\"type\": \"list\", \"map\": {\"id\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"id\", \"leaf-type\": \"lmap:identifier\"}, \"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"string\"}, \"value\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"value\", \"leaf-type\": \"string\"}}, \"section\": \"task_option\", \"leaf-as-name\": \"id\", \"keys\": [\"id\"]}, \"destination\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"destination\", \"leaf-type\": \"schedule-ref\"}, \"tag\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"tag\", \"leaf-type\": \"lmap:tag\"}, \"suppression-tag\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"suppression_tag\", \"leaf-type\": \"lmap:tag\"}}, \"section\": \"action\", \"leaf-as-name\": \"name\", \"keys\": [\"name\"], \"mandatory\": [\"task\", \"state\", \"storage\", \"invocations\", \"suppressions\", \"overlaps\", \"failures\", \"last-invocation\", \"last-completion\", \"last-status\", \"last-message\", \"last-failed-completion\", \"last-failed-status\", \"last-failed-message\"]}}, \"section\": \"schedule\", \"leaf-as-name\": \"name\", \"keys\": [\"name\"], \"mandatory\": [\"start\", \"state\", \"storage\", \"invocations\", \"suppressions\", \"overlaps\", \"failures\"]}}, \"section\": \"schedules\", \"section-name\": \"schedules\"}, \"suppressions\": {\"type\": \"container\", \"map\": {\"suppression\": {\"type\": \"list\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"lmap:identifier\"}, \"start\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"start\", \"leaf-type\": \"event-ref\"}, \"end\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"end\", \"leaf-type\": \"event-ref\"}, \"stop-running\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"stop_running\", \"leaf-type\": \"boolean\"}, \"state\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"state\", \"leaf-type\": \"enumeration\", \"mandatory\": true}, \"match\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"match\", \"leaf-type\": \"lmap:glob-pattern\"}}, \"section\": \"suppression\", \"leaf-as-name\": \"name\", \"keys\": [\"name\"], \"mandatory\": [\"state\"]}}, \"section\": \"suppressions\", \"section-name\": \"suppressions\"}, \"events\": {\"type\": \"container\", \"map\": {\"event\": {\"type\": \"list\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"name\", \"leaf-type\": \"lmap:identifier\"}, \"random-spread\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"random_spread\", \"leaf-type\": \"uint32\"}, \"cycle-interval\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"cycle_interval\", \"leaf-type\": \"uint32\"}, \"event-type\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"event_type\", \"leaf-type\": \"uint32\"}, \"interval\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"interval\", \"leaf-type\": {\"leaf-type\": \"uint32\", \"from\": 1, \"to\": 4294967295}, \"mandatory\": true}, \"timezone-offset\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"timezone_offset\", \"leaf-type\": \"lmap:timezone-offset\"}, \"time\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"time\", \"leaf-type\": \"yang:date-and-time\", \"mandatory\": true}, \"immediate\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"immediate\", \"leaf-type\": \"empty\", \"mandatory\": true}, \"startup\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"startup\", \"leaf-type\": \"empty\", \"mandatory\": true}, \"controller-lost\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"controller_lost\", \"leaf-type\": \"empty\", \"mandatory\": true}, \"controller-connected\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"controller_connected\", \"leaf-type\": \"empty\", \"mandatory\": true}, \"month\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"month\", \"leaf-type\": \"lmap:month-or-all\"}, \"day-of-month\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"day_of_month\", \"leaf-type\": \"lmap:day-of-months-or-all\"}, \"day-of-week\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"day_of_week\", \"leaf-type\": \"lmap:weekday-or-all\"}, \"hour\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"hour\", \"leaf-type\": \"lmap:hour-or-all\"}, \"minute\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"minute\", \"leaf-type\": \"lmap:minute-or-all\"}, \"second\": {\"type\": \"leaf-list\", \"map\": {}, \"option\": \"second\", \"leaf-type\": \"lmap:second-or-all\"}, \"start\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"start\", \"leaf-type\": \"yang:date-and-time\"}, \"end\": {\"type\": \"leaf\", \"map\": {}, \"option\": \"end\", \"leaf-type\": \"yang:date-and-time\"}}, \"section\": \"event\", \"leaf-as-name\": \"name\", \"keys\": [\"name\"], \"mandatory\": [\"interval\", \"time\", \"immediate\", \"startup\", \"controller-lost\", \"controller-connected\"]}}, \"section\": \"events\", \"section-name\": \"events\"}}, \"section\": \"lmap\", \"section-name\": \"lmap\"}}, \"package\": \"lmapd\"}"}
};

static const map_str2str yang2regex[] = {
    {"grade", "{\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"100\"}"},
    {"email", "{\"leaf-type\": \"string\", \"pattern\": \"^[A-Za-z0-9]*@university.de$\"}"},
    {"event-ref", "{\"leaf-type\": \"leafref\"}"},
    {"task-ref", "{\"leaf-type\": \"leafref\"}"},
    {"schedule-ref", "{\"leaf-type\": \"leafref\"}"},
    {"lmap:identifier", "{\"leaf-type\": \"string\"}"},
    {"lmap:tag", "{\"leaf-type\": \"string\"}"},
    {"lmap:glob-pattern", "{\"leaf-type\": \"string\"}"},
    {"lmap:month-or-all", "{\"leaf-type\": \"union\"}"},
    {"lmap:day-of-months-or-all", "{\"leaf-type\": \"union\"}"},
    {"lmap:weekday-or-all", "{\"leaf-type\": \"union\"}"},
    {"lmap:hour-or-all", "{\"leaf-type\": \"union\"}"},
    {"lmap:minute-or-all", "{\"leaf-type\": \"union\"}"},
    {"lmap:second-or-all", "{\"leaf-type\": \"union\"}"},
    {"lmap:status-code", "{\"leaf-type\": \"int32\"}"},
    {"lmap:timezone-offset", "{\"leaf-type\": \"string\", \"pattern\": [\"^Z|[\\\\+\\\\-]\\\\d{2}:\\\\d{2}$\"]}"},
    {"inet:uri", "{\"leaf-type\": \"string\"}"},
    {"yang:counter32", "{\"leaf-type\": \"uint32\"}"},
    {"yang:gauge64", "{\"leaf-type\": \"uint64\"}"},
    {"yang:date-and-time", "{\"leaf-type\": \"string\", \"pattern\": [\"^\\\\d{4}-\\\\d{2}-\\\\d{2}T\\\\d{2}:\\\\d{2}:\\\\d{2}(\\\\.\\\\d+)?(Z|[\\\\+\\\\-]\\\\d{2}:\\\\d{2})$\"]}"},
    {"yang:uuid", "{\"leaf-type\": \"string\", \"pattern\": [\"^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$\"]}"}
};

enum yang_type {
  NONE,
  BINARY,
  BITS,
  BOOLEAN,
  DECIMAL_64,
  EMPTY,
  ENUMERATION,
  IDENTITY_REF,
  INSTANCE_IDENTIFIER,
  INT_8,
  INT_16,
  INT_32,
  INT_64,
  LEAF_REF,
  STRING,
  UINT_8,
  UINT_16,
  UINT_32,
  UINT_64,
  UNION
};
typedef enum yang_type yang_type;

struct map_str2yang_type {
  char* str;
  yang_type type;
};
typedef struct map_str2yang_type map_str2yang_type;

static const map_str2yang_type str2yang_type[] = {
    {"string", STRING},
    {"int8", INT_8},
    {"int16", INT_16},
    {"int32", INT_32},
    {"int64", INT_64},
    {"uint8", UINT_8},
    {"uint16", UINT_16},
    {"uint32", UINT_32},
    {"uint64", UINT_64},
    {"binary", BINARY},
    {"boolean", BOOLEAN},
    {"decimal64", DECIMAL_64},
    {"enumeration", ENUMERATION},
    {"union", UNION},
    {"bits", BITS},
    {"yang:xpath1.0", STRING},
    {"counter32", INT_32},
    {"leafref", LEAF_REF}
};

struct json_object* yang_module_exists(char* module);
yang_type str_to_yang_type(const char* str);
const char* yang_for_type(const char* type);

#endif