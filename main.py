import xmltodict
import json
from mako.template import Template

val = xmltodict.parse("""<?xml version="1.0" encoding="UTF-8"?>
<module name="restconf-example"
        xmlns="urn:ietf:params:xml:ns:yang:yin:1"
        xmlns:ex="http://example.org/example-last-modified">
  <namespace uri="http://example.org/example-last-modified"/>
  <prefix value="ex"/>
  <extension name="uci">
    <argument name="name"/>
  </extension>
  <ex:uci name="restconf"/>
  <container name="state">
    <ex:uci name="state"/>
    <leaf-list name="op">
      <ex:uci name="op"/>
      <type name="string"/>
    </leaf-list>
  </container>
</module>
""")

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
js = js["module"]

def convert(val):
  generated = {}
  for key, value in val.items():
    if key in ["@name", "ex:uci", "container", "type", "leaf-list"]:
      if (isinstance(value, dict)):
        generated[key] = convert(value)
      else:
        generated[key] = value
  return generated

js = convert(js)

modules = [("restconf-example", js)]

print(c.render(modules=modules))
