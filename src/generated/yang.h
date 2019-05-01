

#ifndef _YANG_H
#define _YANG_H
#include <json.h>
#include <stddef.h>
#include <string.h>

struct map_str2str {
  char* key;
  char* str;
};
typedef struct map_str2str map_str2str;

static const map_str2str modulemap[] = {
    {"openwrt-system",
     "{\"type\": \"module\", \"map\": {\"system\": {\"type\": \"container\", "
     "\"map\": {\"hostname\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"hostname\", \"leaf-type\": \"string\"}, "
     "\"buffersize\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"buffersize\", \"leaf-type\": \"uint32\"}, \"conloglevel\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"conloglevel\", \"leaf-type\": "
     "{\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"7\"}}, "
     "\"cronloglevel\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"cronloglevel\", \"leaf-type\": \"uint8\"}, \"klogconloglevel\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"klogconloglevel\", "
     "\"leaf-type\": \"uint8\"}, \"log_buffer_size\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"lof_buffer_size\", \"leaf-type\": "
     "\"uint32\"}, \"log_file\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_file\", \"leaf-type\": \"string\"}, "
     "\"log_hostname\": {\"type\": \"leaf\", \"map\": {}, \"leaf-type\": "
     "\"string\"}, \"log_ip\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_ip\", \"leaf-type\": \"inet:ipv4-address\"}, "
     "\"log_port\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"log_port\", \"leaf-type\": \"uint32\"}, \"log_prefix\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"log_prefix\", \"leaf-type\": "
     "\"string\"}, \"log_proto\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_proto\", \"leaf-type\": {\"leaf-type\": "
     "\"string\", \"pattern\": \"^(udp|tcp)$\"}}, \"log_remote\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"log_remote\", \"leaf-type\": "
     "\"boolean\"}, \"log_size\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"log_size\", \"leaf-type\": \"uint32\"}, "
     "\"log_trailer_null\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"log_trailer_null\", \"leaf-type\": \"boolean\"}, \"log_type\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"log_type\", "
     "\"leaf-type\": {\"leaf-type\": \"string\", \"pattern\": "
     "\"^(circular|file)$\"}}, \"urandom_seed\": {\"type\": \"leaf\", \"map\": "
     "{}, \"uci-option\": \"urandom_seed\", \"leaf-type\": \"string\"}, "
     "\"timezone\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"timezone\", \"leaf-type\": \"string\"}, \"zonename\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"zonename\", \"leaf-type\": "
     "\"string\"}}, \"uci-section\": \"OpenWrt\", \"uci-section-type\": "
     "\"system\"}, \"ntp\": {\"type\": \"container\", \"map\": {\"enabled\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"enabled\", "
     "\"leaf-type\": {\"leaf-type\": \"string\", \"pattern\": \"^(0|1)$\"}}, "
     "\"enable_server\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"enable_server\", \"leaf-type\": {\"leaf-type\": \"string\", "
     "\"pattern\": \"^(0|1)$\"}}, \"server\": {\"type\": \"leaf-list\", "
     "\"map\": {}, \"uci-option\": \"server\", \"leaf-type\": \"string\"}}, "
     "\"uci-section-type\": \"timeserver\", \"uci-section\": \"ntp\"}}, "
     "\"uci-package\": \"system\"}"},
    {"restconf-example",
     "{\"type\": \"module\", \"map\": {\"course\": {\"type\": \"container\", "
     "\"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"name\", \"leaf-type\": \"string\"}, \"semester\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"semester\", \"leaf-type\": "
     "{\"leaf-type\": \"uint8\", \"from\": \"1\", \"to\": \"6\"}}, "
     "\"instructors\": {\"type\": \"leaf-list\", \"map\": {}, \"uci-option\": "
     "\"instructors\", \"leaf-type\": \"string\"}, \"students\": {\"type\": "
     "\"list\", \"map\": {\"firstname\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"firstname\", \"leaf-type\": \"string\"}, \"lastname\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"lastname\", "
     "\"leaf-type\": \"string\"}, \"age\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"age\", \"leaf-type\": {\"leaf-type\": \"uint8\", "
     "\"from\": \"0\", \"to\": \"120\"}}, \"major\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"major\", \"leaf-type\": {\"leaf-type\": "
     "\"string\", \"pattern\": \"^(CS|IMS)$\"}}, \"grade\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"grade\", \"leaf-type\": "
     "\"grade\"}}, \"uci-section-type\": \"student\", \"keys\": "
     "[\"firstname\", \"lastname\", \"age\"]}, \"instructor\": {\"type\": "
     "\"container\", \"map\": {\"name\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"name\", \"leaf-type\": \"string\"}, \"email\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"email\", "
     "\"leaf-type\": \"email\"}}, \"uci-section\": \"instructor\", "
     "\"uci-section-type\": \"instructor\"}}, \"uci-section\": \"course\", "
     "\"uci-section-type\": \"course\"}}, \"uci-package\": "
     "\"restconf-example\"}"},
    {"openwrt-network",
     "{\"type\": \"module\", \"map\": {\"globals\": {\"type\": \"container\", "
     "\"map\": {\"ula_prefix\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"ula_prefix\", \"leaf-type\": \"inet:ipv6-prefix\"}}, "
     "\"uci-section-type\": \"globals\", \"uci-section\": \"globals\"}, "
     "\"interfaces\": {\"type\": \"container\", \"map\": {\"interface\": "
     "{\"type\": \"list\", \"map\": {\"ifname\": {\"type\": \"leaf\", \"map\": "
     "{}, \"uci-option\": \"ifname\", \"leaf-type\": \"string\", "
     "\"mandatory\": true}, \"iftype\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"type\", \"leaf-type\": \"string\"}, \"stp\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"stp\", "
     "\"leaf-type\": \"openwrt-bool\"}, \"bridge_empty\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"bridge_empty\", \"leaf-type\": "
     "\"openwrt-bool\"}, \"igmp_snooping\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"imgp_snooping\", \"leaf-type\": \"openwrt-bool\"}, "
     "\"multicast_querier\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"multicast_querier\", \"leaf-type\": \"openwrt-bool\"}, \"macaddr\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"macaddr\", "
     "\"leaf-type\": \"yang:phys-address\"}, \"mtu\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"mtu\", \"leaf-type\": \"int64\"}, "
     "\"auto\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"auto\", "
     "\"leaf-type\": \"openwrt-bool\"}, \"ipv6\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"ipv6\", \"leaf-type\": \"openwrt-bool\"}, "
     "\"force_link\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"force_link\", \"leaf-type\": \"openwrt-bool\"}, \"disabled\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"disabled\", "
     "\"leaf-type\": \"openwrt-bool\"}, \"ip4table\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"ip4table\", \"leaf-type\": \"string\"}, "
     "\"ip6table\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"ip6table\", \"leaf-type\": \"string\"}, \"proto\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"proto\", \"leaf-type\": "
     "\"string\"}, \"ipaddr\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"ipaddr\", \"leaf-type\": \"inet:ipv4-address\"}, "
     "\"netmask\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"netmask\", \"leaf-type\": \"inet:ipv4-prefix\"}, \"gateway\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"gateway\", "
     "\"leaf-type\": \"inet:ipv4-address\"}, \"broadcast\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"broadcast\", \"leaf-type\": "
     "\"inet:ipv4-address\"}, \"ip6addr\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"ip6addr\", \"leaf-type\": \"inet:ipv6-address\"}, "
     "\"ip6gw\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"ip6gw\", "
     "\"leaf-type\": \"inet:ipv6-address\"}, \"layer\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"layer\", \"leaf-type\": {\"leaf-type\": "
     "\"uint8\", \"from\": \"1\", \"to\": \"3\"}}, \"dns\": {\"type\": "
     "\"leaf-list\", \"map\": {}, \"uci-option\": \"dns\", \"leaf-type\": "
     "\"inet:ipv4-address\"}}, \"uci-section-type\": \"interface\", \"keys\": "
     "[\"ifname\"], \"mandatory\": [\"ifname\"]}}, \"uci-section-type\": "
     "\"interfaces\", \"uci-section\": \"interfaces\"}, \"rules\": {\"type\": "
     "\"container\", \"map\": {\"ip4rules\": {\"type\": \"list\", \"map\": "
     "{\"mark\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"mark\", "
     "\"leaf-type\": \"string\"}, \"in\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"in\", \"leaf-type\": \"string\"}, \"out\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"out\", \"leaf-type\": "
     "\"string\"}, \"src\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"src\", \"leaf-type\": \"inet:ipv4-prefix\"}, \"dest\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"dest\", \"leaf-type\": "
     "\"inet:ipv4-prefix\"}, \"tos\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"tos\", \"leaf-type\": \"uint64\"}, \"invert\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"invert\", "
     "\"leaf-type\": \"openwrt-bool\"}, \"priority\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"priority\", \"leaf-type\": \"uint64\"}, "
     "\"lookup\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"lookup\", \"leaf-type\": \"string\"}, \"goto\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"goto\", \"leaf-type\": \"uint64\"}, "
     "\"action\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"action\", \"leaf-type\": \"string\"}}, \"uci-section-type\": \"rule\", "
     "\"keys\": [\"mark\"]}, \"ip6rules\": {\"type\": \"list\", \"map\": "
     "{\"mark\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"mark\", "
     "\"leaf-type\": \"string\"}, \"in\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"in\", \"leaf-type\": \"string\"}, \"out\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"out\", \"leaf-type\": "
     "\"string\"}, \"src\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"src\", \"leaf-type\": \"inet:ipv6-prefix\"}, \"dest\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"dest\", \"leaf-type\": "
     "\"inet:ipv6-prefix\"}, \"tos\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"tos\", \"leaf-type\": \"uint64\"}, \"invert\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"invert\", "
     "\"leaf-type\": \"openwrt-bool\"}, \"priority\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"priority\", \"leaf-type\": \"uint64\"}, "
     "\"lookup\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"lookup\", \"leaf-type\": \"string\"}, \"goto\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"goto\", \"leaf-type\": \"uint64\"}, "
     "\"action\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"action\", \"leaf-type\": \"string\"}}, \"uci-section-type\": \"rule\", "
     "\"keys\": [\"mark\"]}}, \"uci-section-type\": \"rules\", "
     "\"uci-section\": \"rules\"}, \"routes\": {\"type\": \"container\", "
     "\"map\": {\"ip4routes\": {\"type\": \"list\", \"map\": {\"name\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"name\", "
     "\"leaf-type\": \"string\"}, \"interface\": {\"type\": \"leaf\", \"map\": "
     "{}, \"uci-option\": \"interface\", \"leaf-type\": \"string\", "
     "\"mandatory\": true}, \"target\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"target\", \"leaf-type\": \"inet:ipv4-address\"}, "
     "\"netmask\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"netmask\", \"leaf-type\": \"inet:ipv4-prefix\"}, \"gateway\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"gateway\", "
     "\"leaf-type\": \"inet:ipv4-address\"}, \"metric\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"metric\", \"leaf-type\": \"uint64\"}, "
     "\"mtu\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"mtu\", "
     "\"leaf-type\": \"uint64\"}, \"table\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"table\", \"leaf-type\": \"string\"}, \"source\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"source\", "
     "\"leaf-type\": \"inet:ipv4-address\"}, \"onlink\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"onlink\", \"leaf-type\": "
     "\"openwrt-bool\"}, \"rtype\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"type\", \"leaf-type\": \"string\"}}, "
     "\"uci-section-type\": \"route\", \"keys\": [\"name\"], \"mandatory\": "
     "[\"interface\"]}, \"ip6routes\": {\"type\": \"list\", \"map\": "
     "{\"name\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"name\", "
     "\"leaf-type\": \"string\"}, \"interface\": {\"type\": \"leaf\", \"map\": "
     "{}, \"uci-option\": \"interface\", \"leaf-type\": \"string\", "
     "\"mandatory\": true}, \"target\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"target\", \"leaf-type\": \"inet:ipv6-address\"}, "
     "\"gateway\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"gateway\", \"leaf-type\": \"inet:ipv6-address\"}, \"metric\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"metric\", "
     "\"leaf-type\": \"uint64\"}, \"mtu\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"mtu\", \"leaf-type\": \"uint64\"}, \"table\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"table\", "
     "\"leaf-type\": \"string\"}, \"source\": {\"type\": \"leaf\", \"map\": "
     "{}, \"uci-option\": \"source\", \"leaf-type\": \"inet:ipv4-address\"}, "
     "\"onlink\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"onlink\", \"leaf-type\": \"openwrt-bool\"}, \"rtype\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"type\", \"leaf-type\": "
     "\"string\"}}, \"uci-section-type\": \"route6\", \"keys\": [\"name\"], "
     "\"mandatory\": [\"interface\"]}}, \"uci-section-type\": \"routes\", "
     "\"uci-section\": \"routes\"}, \"switches\": {\"type\": \"container\", "
     "\"map\": {\"switch\": {\"type\": \"list\", \"map\": {\"name\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"name\", "
     "\"leaf-type\": \"string\"}, \"reset\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"reset\", \"leaf-type\": \"openwrt-bool\"}, "
     "\"enable_vlan\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"enable_vlan\", \"leaf-type\": \"openwrt-bool\"}, \"enable_mirror_rx\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"enable_mirror_rx\", "
     "\"leaf-type\": \"openwrt-bool\"}, \"enable_mirror_tx\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"enable_mirror_tx\", "
     "\"leaf-type\": \"openwrt-bool\"}, \"mirror_monitor_port\": {\"type\": "
     "\"leaf\", \"map\": {}, \"uci-option\": \"mirror_monitor_port\", "
     "\"leaf-type\": \"uint64\"}, \"mirror_source_port\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"mirror_source_port\", \"leaf-type\": "
     "\"uint64\"}, \"arl_age_time\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"arl_age_time\", \"leaf-type\": \"uint64\"}, "
     "\"igmp_snooping\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"igmp_snooping\", \"leaf-type\": \"openwrt-bool\"}, \"igmp_v3\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"igmp_v3\", "
     "\"leaf-type\": \"openwrt-bool\"}}, \"uci-section-type\": \"switch\", "
     "\"keys\": [\"name\"]}, \"switch_vlan\": {\"type\": \"list\", \"map\": "
     "{\"device\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"device\", \"leaf-type\": \"string\"}, \"vlan\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"vlan\", \"leaf-type\": \"uint64\"}, "
     "\"vid\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"vid\", "
     "\"leaf-type\": \"uint64\"}, \"ports\": {\"type\": \"leaf-list\", "
     "\"map\": {}, \"uci-option\": \"ports\", \"leaf-type\": \"string\"}}, "
     "\"uci-section-type\": \"switch_vlan\", \"keys\": [\"device\", "
     "\"vlan\"]}, \"switch_port\": {\"type\": \"list\", \"map\": {\"device\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"device\", "
     "\"leaf-type\": \"string\"}, \"port\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"port\", \"leaf-type\": \"uint64\"}, \"pvid\": "
     "{\"type\": \"leaf\", \"map\": {}, \"uci-option\": \"pvid\", "
     "\"leaf-type\": \"uint64\"}, \"enable_eee\": {\"type\": \"leaf\", "
     "\"map\": {}, \"uci-option\": \"enable_eee\", \"leaf-type\": "
     "\"openwrt-bool\"}, \"igmp_snooping\": {\"type\": \"leaf\", \"map\": {}, "
     "\"uci-option\": \"igmp_snooping\", \"leaf-type\": \"openwrt-bool\"}, "
     "\"igmp_v3\": {\"type\": \"leaf\", \"map\": {}, \"uci-option\": "
     "\"igmp_v3\", \"leaf-type\": \"openwrt-bool\"}}, \"uci-section-type\": "
     "\"switch_port\", \"keys\": [\"device\", \"port\"]}}, "
     "\"uci-section-type\": \"switches\", \"uci-section\": \"switches\"}}, "
     "\"uci-package\": \"network\"}"}};

static const map_str2str yang2regex[] = {
    {"grade", "{\"leaf-type\": \"uint8\", \"from\": \"0\", \"to\": \"100\"}"},
    {"email",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "\"^[A-Za-z0-9]*@university.de$\"}"},
    {"openwrt-bool", "{\"leaf-type\": \"string\", \"pattern\": \"^(0|1)$\"}"},
    {"inet:ipv4-address",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "[\"^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\\\.){3}([0-9]|["
     "1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])(%[\\\\p{N}\\\\p{L}]+)?$\"]}"},
    {"inet:ipv6-address",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "[\"^((:|[0-9a-fA-F]{0,4}):)([0-9a-fA-F]{0,4}:){0,5}((([0-9a-fA-F]{0,4}:)?"
     "(:|[0-9a-fA-F]{0,4}))|(((25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])\\\\.){3}("
     "25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])))(%[\\\\p{N}\\\\p{L}]+)?$\", "
     "\"^(([^:]+:){6}(([^:]+:[^:]+)|(.*\\\\..*)))|((([^:]+:)*[^:]+)?::(([^:]+:)"
     "*[^:]+)?)(%.+)?$\"]}"},
    {"inet:ipv4-prefix",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "[\"^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\\\.){3}([0-9]|["
     "1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])/"
     "(([0-9])|([1-2][0-9])|(3[0-2]))$\"]}"},
    {"inet:ipv6-prefix",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "[\"^((:|[0-9a-fA-F]{0,4}):)([0-9a-fA-F]{0,4}:){0,5}((([0-9a-fA-F]{0,4}:)?"
     "(:|[0-9a-fA-F]{0,4}))|(((25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])\\\\.){3}("
     "25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])))(/"
     "(([0-9])|([0-9]{2})|(1[0-1][0-9])|(12[0-8])))$\", "
     "\"^(([^:]+:){6}(([^:]+:[^:]+)|(.*\\\\..*)))|((([^:]+:)*[^:]+)?::(([^:]+:)"
     "*[^:]+)?)(/.+)$\"]}"},
    {"yang:phys-address",
     "{\"leaf-type\": \"string\", \"pattern\": "
     "[\"^([0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*)?$\"]}"}};

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
    {"union", UNION}};

struct json_object* yang_module_exists(char* module);
yang_type str_to_yang_type(const char* str);
const char* yang_for_type(const char* type);

#endif
