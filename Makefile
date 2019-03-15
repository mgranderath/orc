CC=../openwrt/staging_dir/toolchain-x86_64_gcc-7.4.0_musl/bin/x86_64-openwrt-linux-gcc

CC_FLAGS=

JSON_C=../openwrt/build_dir/target-x86_64_musl/json-c-0.12.1/
UCI=../openwrt/build_dir/target-x86_64_musl/uci-2018-08-11-4c8b4d6e

INC=$(JSON_C) $(UCI)
INC_PARAMS=$(foreach d, $(INC), -I$d)

LIB=-L../openwrt/staging_dir/target-x86_64_musl/usr/lib/

build/restconf: src/*
	mkdir -p ./build
	$(CC) $(CC_FLAGS) $(INC_PARAMS) $(LIB) -o build/restconf src/restconf.c src/http.c src/cgi.c src/restconf-method.c -ljson-c

clean:
	rm *.o restconf