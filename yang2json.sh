#!/bin/bash

declare -a f_array=("openwrt-uci-extension" "ietf-inet-types" "ietf-yang-types" "ietf-netconf-acm" "ietf-lmap-common" "lmapd" "restconf-example")

for val in ${f_array[@]}; do
    echo "pyang -f yin ./yang/$val.yang -p ./yang -o ./yin/$val.yin"
    pyang -f yin ./yang/$val.yang -p ./yang -o ./yin/$val.yin
done

target=$1

echo "python3 ./yin2json/yin2json.py -y ./yin -o ./generated ./yin/$target.yin"
python3 ./yin2json/yin2json.py -y ./yin -o ./generated ./yin/$target.yin
