import argparse
import json
import os

import xmltodict
from mako.template import Template

parser = argparse.ArgumentParser(description='Preprocess YIN for OpenWrt RESTCONF')

parser.add_argument("-y", action="store", dest="yin_dir", help="specify YIN directory", required=True)
parser.add_argument("file", action="store", nargs="+", help="The YIN file for input")

args = parser.parse_args()

with open(args.file[0]) as file:
    data = file.read()

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

# print(json.dumps(js, indent=4))

importedModules = {}
importedTypes = {}
openwrt_prefix = ""
types = {}
allowed_types = {
    "string": True,
    "int8": True,
    "int16": True,
    "int32": True,
    "int64": True,
    "uint8": True,
    "uint16": True,
    "uint32": True,
    "uint64": True,
    "binary": True,
    "boolean": True,
    "decimal64": True
}


def convert_yin_to_json(data):
    val = xmltodict.parse(data)
    return json.loads(json.dumps(val))


def range_allowed(name):
    return name in ["int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint43"]


def handle_import(js):
    global openwrt_prefix
    if isinstance(js, dict):
        name = js["@module"]
        prefix = js["prefix"]["@value"]
        importedModules[prefix] = name
        if name == "openwrt-uci-extension":
            openwrt_prefix = prefix
    elif isinstance(js, list):
        for item in js:
            name = item["@module"]
            prefix = item["prefix"]["@value"]
            importedModules[prefix] = name
            if name == "openwrt-uci-extension":
                openwrt_prefix = prefix


def extract_uci_statements(generated, key, value):
    if key == openwrt_prefix + ":uci-package":
        generated["uci-package"] = value["@name"]
    if key == openwrt_prefix + ":uci-section":
        generated["uci-section"] = value["@name"]
    if key == openwrt_prefix + ":uci-option":
        generated["uci-option"] = value["@name"]
    if key == openwrt_prefix + ":uci-section-type":
        generated["uci-section-type"] = value["@name"]
    return generated


def convert(level, type=None):
    generated = {}
    changed_level = level
    if len(changed_level) == 1 and "module" in changed_level:
        generated["type"] = "module"
        changed_level = level["module"]
        if "import" in changed_level:
            handle_import(changed_level["import"])
    if type is not None:
        generated["type"] = type
    generated["map"] = {}
    for key, value in changed_level.items():
        extract_uci_statements(generated, key, value)
        if key == "type":
            type_name = value["@name"]
            if ":" in type_name:
                split = type_name.split(":", 1)
                if split[0] not in importedModules:
                    raise Exception("Import type from module that is not defined")
                importedTypes[type_name] = {
                    "module": importedModules[split[0]],
                    "type": split[1]
                }
            elif type_name not in allowed_types:
                raise Exception("Unsupported type")
            if type_name == "string" and "pattern" in value:
                type_name = {
                    "leaf-type": type_name,
                    "pattern": "^" + value["pattern"]["@value"] + "$"
                }
            if range_allowed(type_name) and "range" in value:
                range_split = value["range"]["@value"].split("..", 1)
                type_name = {
                    "leaf-type": type_name,
                    "from": range_split[0],
                    "to": range_split[1]
                }
            generated["leaf-type"] = type_name
        if key == "key":
            to_be_split = value["@value"]
            generated["keys"] = to_be_split.split()
        if key == "mandatory":
            generated["mandatory"] = value["@value"] == "true"
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


js = convert_yin_to_json(data)
js = convert(js)

for key, value in importedTypes.items():
    with open(os.path.join(args.yin_dir, value["module"] + ".yin")) as file:
        data = file.read()
        mod = convert_yin_to_json(data)["module"]
        if "typedef" not in mod:
            raise Exception("Type is not declared in module")
        typedef = mod["typedef"]
        if isinstance(typedef, list):
            for item in typedef:
                if item["@name"] == value["type"]:
                    converted = {
                        "leaf-type": item["type"]["@name"]
                    }
                    if converted["leaf-type"] == "string" and "pattern" in item["type"]:
                        converted["pattern"] = "^" + item["type"]["pattern"]["@value"] + "$"
                    if range_allowed(converted["leaf-type"]) and "range" in item["type"]:
                        range_split = item["type"]["range"]["@value"].split("..", 1)
                        converted["from"] = range_split[0]
                        converted["to"] = range_split[1]
                    types[key] = converted


modules = [("restconf-example", js)]

# print(c.render(modules=modules))
print(json.dumps(js))
print(json.dumps(types))
