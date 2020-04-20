#!/bin/bash

declare -a f_array=("openwrt-uci-extension" "ietf-yang-types" "ietf-netconf-acm" "ietf-lmap-common" "lmapd" "restconf-example")

for val in ${f_array[@]}; do
    echo "pyang -f yin ./yang/$val.yang -p ./yang -o ./yin/$val.yin"
    pyang -f yin ./yang/$val.yang -p ./yang -o ./yin/$val.yin
done

all_files=""

for val in ${f_array[@]}; do
    all_files+="./yin/$val.yin "
done

echo "python3 ./yin2json/yin2json.py -y ./yin -o ./generated $all_files"
python3 ./yin2json/yin2json.py -y ./yin -o ./generated $all_files
