import json
import sys

import xmltodict
from mako.template import Template

data = sys.stdin.read()

val = xmltodict.parse(data)

c = Template("""
#ifndef _YANG_H
#define _YANG_H
#include <stddef.h>

struct map_str2str{print(js)
  char *key;
  char *str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
  % for name, json in modules:
  {"${name}", "${json}"},
  % endfor
  {NULL, ""}
};
#endif
""")

js = json.loads(json.dumps(val))


def convert(level, type=None):
    generated = {}
    changed_level = level
    if len(changed_level) == 1 and "module" in changed_level:
        generated["type"] = "module"
        changed_level = level["module"]
    if type is not None:
        generated["type"] = type
    generated["map"] = {}
    for key, value in changed_level.items():
        if key == "ex:uci-package":
            generated["uci-package"] = value["@name"]
        if key == "ex:uci-section":
            generated["uci-section"] = value["@name"]
        if key == "ex:uci-option":
            generated["uci-option"] = value["@name"]
        if key == "ex:uci-section-type":
            generated["uci-section-type"] = value["@name"]
        if key == "type":
            generated["leaf-type"] = value["@name"]
        if key == "key":
            to_be_split = value["@value"]
            generated["keys"] = to_be_split.split()
        if key in ["container", "leaf", "leaf-list", "list"]:
            if isinstance(value, dict):
                inner_key = value["@name"]
                if "mandatory" in value and value["mandatory"]["@value"] == "true":
                    if not ("mandatory" in generated):
                        generated["mandatory"] = []
                    generated["mandatory"].append(inner_key)
                generated["map"][inner_key] = convert(value, key)
            elif isinstance(value, list):
                for inner_value in value:
                    if "mandatory" in inner_value and inner_value["mandatory"]["@value"] == "true":
                        if not ("mandatory" in generated):
                            generated["mandatory"] = []
                        generated["mandatory"].append(inner_value["@name"])
                    inner_key = inner_value["@name"]
                    generated["map"][inner_key] = convert(inner_value, key)
    return generated


js = convert(js)

modules = [("restconf-example", js)]

# print(c.render(modules=modules))
print(json.dumps(js))
