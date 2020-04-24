import argparse
import json
import os

import xmltodict
from mako.lookup import TemplateLookup
from mako.template import Template

dirname = os.path.dirname(os.path.realpath(__file__))
mylookup = TemplateLookup(directories=[os.path.join(dirname, "./template")])
header_file = Template(filename=os.path.join(dirname, "./template/yang.h.templ"), lookup=mylookup)

types = {}
ALLOWED_TYPES = {
    "string": "STRING",
    "int8": "INT_8",
    "int16": "INT_16",
    "int32": "INT_32",
    "int64": "INT_64",
    "uint8": "UINT_8",
    "uint16": "UINT_16",
    "uint32": "UINT_32",
    "uint64": "UINT_64",
    "binary": "BINARY",
    "boolean": "BOOLEAN",
    "decimal64": "DECIMAL_64",
    "enumeration": "ENUMERATION",
    "union": "UNION",
    "bits": "BITS",
    "yang:xpath1.0": "STRING",
    "counter32": "INT_32",
    "leafref": "LEAF_REF",
    "empty": "EMPTY"
}


class Imported:
    def __init__(self):
        self.modules = {}
        self.types = {}
        self.openwrt_prefix = ""

    def set_openwrt_prefix(self, prefix):
        self.openwrt_prefix = prefix

    def add_module(self, prefix, name):
        self.modules[prefix] = name

    def get_modules(self):
        return self.modules

    def get_module(self, prefix):
        if prefix not in self.modules:
            return ""
        return self.modules[prefix]

    def add_type(self, module_name, content):
        if module_name not in self.types:
            self.types[module_name] = []
        self.types[module_name].append(content)

    def get_types(self):
        return self.types

class Grouping:
    def __init__(self):
        self.tree = {}
        self.current_size = 0

    def generate_name(self):
        return "grouping{}".format(self.current_size)

    def add_grouping(self, name, value={}):
        if name in self.tree:
            print("Duplicate grouping: {}\nPlease check YANG model".format(name))
            return
        self.tree[name] = value
        self.current_size += 1

    def find(self, name):
        if name in self.tree:
            return self.tree[name]["map"]
        return None
    
def convert_yin_to_json(data):
    val = xmltodict.parse(data)
    return json.loads(json.dumps(val))


def range_allowed(name):
    return name in ["int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64"]


def handle_import(js, imported):
    if isinstance(js, dict):
        name = js["@module"]
        prefix = js["prefix"]["@value"]
        imported.add_module(prefix, name)
        if name == "openwrt-uci-extension":
            imported.set_openwrt_prefix(prefix)
    elif isinstance(js, list):
        for item in js:
            handle_import(item, imported)


def extract_uci_statements(generated, key, value, imported):
    if key == imported.openwrt_prefix + ":package":
        generated["package"] = value["@name"]
    if key == imported.openwrt_prefix + ":section-name":
        generated["section-name"] = value["@name"]
    if key == imported.openwrt_prefix + ":option":
        generated["option"] = value["@name"]
    if key == imported.openwrt_prefix + ":section":
        generated["section"] = value["@name"]
    if key == imported.openwrt_prefix + ":leaf-as-name":
        generated["leaf-as-name"] = value["@name"]


def extract_type_statements(generated, key, value, imported):
    type_name = value["@name"]
    if ":" in type_name:
        split = type_name.split(":", 1)
        if split[0] not in imported.get_modules():
            raise Exception("Using type from module that is not defined \"{}\"".format(split[0]))
        module_name = imported.get_module(split[0])
        imported.add_type(module_name, {
            "type_name": type_name,
            "type": split[1]
        })
    elif type_name not in ALLOWED_TYPES and type_name not in types:
        raise Exception("Unsupported type \"{}\" used".format(type_name))
    if type_name == "string" and "pattern" in value:
        type_name = {
            "leaf-type": type_name,
            "pattern": "^" + value["pattern"]["@value"] + "$"
        }
    if range_allowed(type_name) and "range" in value:
        range_split = get_range(value["range"]["@value"].split("..", 1), type_name)
        type_name = {
            "leaf-type": type_name,
            "from": range_split[0],
            "to": range_split[1]
        }
    generated["leaf-type"] = type_name

def get_range(range_split, type_name):
    if len(range_split) == 0:
        return [0, 0]
    try:
        val1 = int(range_split[0])
    except ValueError:
        val1 = get_min_or_max(range_split[0], type_name)
    try:
        val2 = int(range_split[1])
    except ValueError:
        val2 = get_min_or_max(range_split[1], type_name)
    return [val1, val2]

def get_min_or_max(x, t):
    if x == "max":
        if t == "int8":
            return (2**7)-1
        elif t == "int16":
            return (2**15)-1
        elif t == "int32":
            return (2**31)-1
        elif t == "int64":
            return (2**63)-1
        elif t == "uint8":
            return (2**8)-1
        elif t == "uint16":
            return (2**16)-1
        elif t == "uint32":
            return (2**32)-1
        else:
            return (2**64)-1
    elif x == "min":
        if t == "int8":
            return -(2**7)
        elif t == "int16":
            return -(2**15)
        elif t == "int32":
            return -(2**31)
        elif t == "int64":
            return -(2**63)
        else:
            return 0
    else:
        return 0


def handle_typedef(typedefs):
    if isinstance(typedefs, dict):
        import_type = typedefs["type"]["@name"]
        if import_type not in ALLOWED_TYPES and import_type not in types:
            raise Exception("Unsupported type in imported type \"{}\"".format(import_type))
        converted = {
            "leaf-type": typedefs["type"]["@name"]
        }
        if converted["leaf-type"] == "string" and "pattern" in typedefs["type"]:
            if isinstance(typedefs["type"]["pattern"], list):
                regex = ""
                for val in typedefs["type"]["pattern"]:
                    regex += "({})|".format(val["@value"])
                converted["pattern"] = "^({})$".format(regex[:len(regex)-1])
            else:
                converted["pattern"] = "^" + typedefs["type"]["pattern"]["@value"] + "$"
        if range_allowed(converted["leaf-type"]) and "range" in typedefs["type"]:
            range_split = get_range(typedefs["type"]["range"]["@value"].split("..", 1), converted["leaf-type"])
            converted["from"] = range_split[0]
            converted["to"] = range_split[1]
        types[typedefs["@name"]] = converted
    elif isinstance(typedefs, list):
        for item in typedefs:
            handle_typedef(item)


def process_node(generated, key, value, imported, groupings):
    if isinstance(value, dict):
        inner_key = value["@name"]
        if "mandatory" in value and value["mandatory"]["@value"] == "true":
            if not ("mandatory" in generated):
                generated["mandatory"] = []
            generated["mandatory"].append(inner_key)
        generated["map"][inner_key] = convert(value, imported, groupings, key)
    elif isinstance(value, list):
        for inner_value in value:
            process_node(generated, key, inner_value, imported, groupings)

def handle_grouping(value, imported, groupings):
    if isinstance(value, dict):
        grouping_name = ""
        if "@name" in value:
            grouping_name = value["@name"]
        else:
            grouping_name = groupings.generate_name()
        groupings.add_grouping(grouping_name, convert(value, imported, groupings, "grouping"))
    elif isinstance(value, list):
        for val in value:
            handle_grouping(val, imported, groupings)

def extract_uses(generated, value, imported, groupings):
    if isinstance(value, list):
        for val in value:
            extract_uses(generated, val, imported, groupings)
        return
    if "@name" not in value:
        print("Invalid use of 'uses': {}".format(value))
        return
    name = value["@name"]
    grouping = groupings.find(name)
    if grouping == None:
        print("Could not find {} in groupings".format(name))
        return
    for key, val in grouping.items():
        if key in generated["map"].keys():
            print("A node with name '{}' already exists".format(key))
            continue
        generated["map"][key] = val

def convert(level, imported, groupings, object_type=None):
    generated = {}
    changed_level = level
    if len(changed_level) == 1 and "module" in changed_level:
        generated["type"] = "module"
        changed_level = level["module"]
        if "import" in changed_level:
            handle_import(changed_level["import"], imported)
        if "typedef" in changed_level:
            handle_typedef(changed_level["typedef"])
    if object_type is not None:
        generated["type"] = object_type
    generated["map"] = {}
    for key, value in changed_level.items():
        extract_uci_statements(generated, key, value, imported)
        if key == "grouping":
            handle_grouping(value, imported, groupings)
        if key == "type":
            extract_type_statements(generated, key, value, imported)
        if key == "key":
            to_be_split = value["@value"]
            generated["keys"] = to_be_split.split()
        if key == "mandatory":
            generated["mandatory"] = value["@value"] == "true"
        if key == "unique":
            to_be_split = value["@value"]
            generated["unique"] = to_be_split.split()
        if key in ["container", "leaf", "leaf-list", "list"]:
            process_node(generated, key, value, imported, groupings)
        if key == "uses":
            extract_uses(generated, value, imported, groupings)
    return generated


def process_imported_types(args, imported):
    for key, value in imported.get_types().items():
        with open(os.path.join(args.yin_dir, key + ".yin")) as file:
            data = file.read()
            mod = convert_yin_to_json(data)["module"]
            if "typedef" not in mod:
                raise Exception("Type is not declared in module \"{}\"".format(mod))
            typedef = mod["typedef"]
            if isinstance(typedef, list):
                for item in typedef:
                    for val in value:
                        if item["@name"] == val["type"]:
                            import_type = item["type"]["@name"]
                            if import_type not in ALLOWED_TYPES and import_type not in types:
                                raise Exception("Unsupported type in imported type {}".format(import_type))
                            converted = {
                                "leaf-type": import_type
                            }
                            if converted["leaf-type"] == "string" and "pattern" in item["type"]:
                                converted["pattern"] = []
                                if isinstance(item["type"]["pattern"], list):
                                    for pattern in item["type"]["pattern"]:
                                        converted["pattern"].append("^" + pattern["@value"] + "$")
                                else:
                                    converted["pattern"].append("^" + item["type"]["pattern"]["@value"] + "$")
                            if range_allowed(converted["leaf-type"]) and "range" in item["type"]:
                                range_split = get_range(item["type"]["range"]["@value"].split("..", 1), converted["leaf-type"])
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
            groupings = Grouping()
            imported = Imported()
            js = convert(js, imported, groupings)
            modules.append((os.path.basename(file).split('.')[0], js))

    process_imported_types(args, imported)

    if not os.path.exists(args.output):
        os.makedirs(args.output)

    with open(os.path.join(args.output, "yang.h"), "w+") as out:
        rendered = header_file.render(modules=modules, types=types, ALLOWED_TYPES=ALLOWED_TYPES)
        out.write(rendered)


if __name__ == '__main__':
    main()
