import argparse
import json
import os

import xmltodict
from mako.template import Template

c = Template("""
<%! import json %>
#ifndef _YANG_H
#define _YANG_H
#include <stddef.h>
#include <json.h>
#include <string.h>

struct map_str2str{
  char *key;
  char *str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
    % for index, (name, content) in enumerate(modules):
    % if index + 1 == len(modules):
    {"${name}", ${json.dumps(json.dumps(content))}}
    % else:
    {"${name}", ${json.dumps(json.dumps(content))}},
    %endif
    % endfor
};

static const map_str2str yang2regex[] = {
    % for index, (key, value) in enumerate(types.items()):
    % if index + 1 == len(types.items()):
    {"${key}", ${json.dumps(json.dumps(value))}}
    % else:
    {"${key}", ${json.dumps(json.dumps(value))}},
    %endif
    % endfor
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
    {"binary", BINARY},
    {"bits", BITS},
    {"boolean", BOOLEAN},
    {"decimal64", DECIMAL_64},
    {"emtpy", EMPTY},
    {"enumeration", ENUMERATION},
    {"identityref", IDENTITY_REF},
    {"instance-identifier", INSTANCE_IDENTIFIER},
    {"int8", INT_8},
    {"int16", INT_16},
    {"int32", INT_32},
    {"int64", INT_64},
    {"leafref", LEAF_REF},
    {"string", STRING},
    {"uint8", UINT_8},
    {"uint16", UINT_16},
    {"uint32", UINT_32},
    {"uint64", UINT_64},
    {"union", UNION}
};

struct json_object* yang_module_exists(char* module);
yang_type str_to_yang_type(const char* str);
const char* yang_for_type(char* type);

#endif
""")

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
    return name in ["int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64"]


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
            handle_import(item)


def extract_uci_statements(generated, key, value):
    if key == openwrt_prefix + ":uci-package":
        generated["uci-package"] = value["@name"]
    if key == openwrt_prefix + ":uci-section":
        generated["uci-section"] = value["@name"]
    if key == openwrt_prefix + ":uci-option":
        generated["uci-option"] = value["@name"]
    if key == openwrt_prefix + ":uci-section-type":
        generated["uci-section-type"] = value["@name"]


def extract_type_statements(generated, key, value):
    type_name = value["@name"]
    if ":" in type_name:
        split = type_name.split(":", 1)
        if split[0] not in importedModules:
            raise Exception("Using type from module that is not defined {}".format(split[0]))
        if importedModules[split[0]] not in importedTypes:
            importedTypes[importedModules[split[0]]] = []
        importedTypes[importedModules[split[0]]].append({
            "type_name": type_name,
            "type": split[1]
        })
    elif type_name not in allowed_types and type_name not in types:
        raise Exception("Unsupported type {} used".format(type_name))
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


def handleTypedef(typedefs):
    if isinstance(typedefs, dict):
        import_type = typedefs["type"]["@name"]
        if import_type not in allowed_types and import_type not in types:
            raise Exception("Unsupported type in imported type {}".format(import_type))
        converted = {
            "leaf-type": typedefs["type"]["@name"]
        }
        if converted["leaf-type"] == "string" and "pattern" in typedefs["type"]:
            converted["pattern"] = "^" + typedefs["type"]["pattern"]["@value"] + "$"
        if range_allowed(converted["leaf-type"]) and "range" in typedefs["type"]:
            range_split = typedefs["type"]["range"]["@value"].split("..", 1)
            converted["from"] = range_split[0]
            converted["to"] = range_split[1]
        types[typedefs["@name"]] = converted
    elif isinstance(typedefs, list):
        for item in typedefs:
            handleTypedef(item)


def processNode(generated, key, value):
    if isinstance(value, dict):
        inner_key = value["@name"]
        if "mandatory" in value and value["mandatory"]["@value"] == "true":
            if not ("mandatory" in generated):
                generated["mandatory"] = []
            generated["mandatory"].append(inner_key)
        generated["map"][inner_key] = convert(value, key)
    elif isinstance(value, list):
        for inner_value in value:
            processNode(generated, key, inner_value)


def convert(level, object_type=None):
    generated = {}
    changed_level = level
    if len(changed_level) == 1 and "module" in changed_level:
        generated["type"] = "module"
        changed_level = level["module"]
        if "import" in changed_level:
            handle_import(changed_level["import"])
        if "typedef" in changed_level:
            handleTypedef(changed_level["typedef"])
    if object_type is not None:
        generated["type"] = object_type
    generated["map"] = {}
    for key, value in changed_level.items():
        extract_uci_statements(generated, key, value)
        if key == "type":
            extract_type_statements(generated, key, value)
        if key == "key":
            to_be_split = value["@value"]
            generated["keys"] = to_be_split.split()
        if key == "mandatory":
            generated["mandatory"] = value["@value"] == "true"
        if key in ["container", "leaf", "leaf-list", "list"]:
            processNode(generated, key, value)
    return generated


def processImportedTypes(args):
    for key, value in importedTypes.items():
        with open(os.path.join(args.yin_dir, key + ".yin")) as file:
            data = file.read()
            mod = convert_yin_to_json(data)["module"]
            if "typedef" not in mod:
                raise Exception("Type is not declared in module")
            typedef = mod["typedef"]
            if isinstance(typedef, list):
                for item in typedef:
                    for val in value:
                        if item["@name"] == val["type"]:
                            import_type = item["type"]["@name"]
                            if import_type not in allowed_types and import_type not in types:
                                raise Exception("Unsupported type in imported type {}".format(import_type))
                            converted = {
                                "leaf-type": item["type"]["@name"]
                            }
                            if converted["leaf-type"] == "string" and "pattern" in item["type"]:
                                converted["pattern"] = "^" + item["type"]["pattern"]["@value"] + "$"
                            if range_allowed(converted["leaf-type"]) and "range" in item["type"]:
                                range_split = item["type"]["range"]["@value"].split("..", 1)
                                converted["from"] = range_split[0]
                                converted["to"] = range_split[1]
                            types[val["type_name"]] = converted


def main():
    parser = argparse.ArgumentParser(description='Preprocess YIN for OpenWrt RESTCONF')

    parser.add_argument("-y", action="store", dest="yin_dir", help="specify YIN directory", required=True)
    parser.add_argument("file", action="store", nargs="+", help="The YIN file for input")
    parser.add_argument("-o", "--output", dest="output", help="The output directory", required=True)

    args = parser.parse_args()

    modules = []

    for file in args.file:
        with open(file) as yin:
            data = yin.read()
            js = convert_yin_to_json(data)
            js = convert(js)
            modules.append((os.path.basename(file).split('.')[0], js))

    processImportedTypes(args)

    if not os.path.exists(args.output):
        os.makedirs(args.output)

    with open(os.path.join(args.output, "yang.h"), "w+") as out:
        rendered = c.render(modules=modules, types=types)
        out.write(rendered)


if __name__ == '__main__':
    main()
