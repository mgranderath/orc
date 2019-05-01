cmake_minimum_required(VERSION 3.13)
project(restconf)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "--coverage")
set(CMAKE_BUILD_TYPE Debug)
set(GCOV_PREFIX "/www/cgi-bin/gcov")
set(STAGING_DIR $ENV{HOME}/github/openwrt/staging_dir)
set(CMAKE_C_COMPILER $ENV{HOME}/github/openwrt/staging_dir/toolchain-x86_64_gcc-7.4.0_musl/bin/x86_64-openwrt-linux-gcc)

include_directories(src)
include_directories($ENV{HOME}/github/openwrt/build_dir/target-x86_64_musl/json-c-0.12.1/)
include_directories($ENV{HOME}/github/openwrt/build_dir/target-x86_64_musl/uci-2018-08-11-4c8b4d6e)
include_directories($ENV{HOME}/github/openwrt/build_dir/target-x86_64_musl/ubox-2018-12-18-876c7f5b/)

link_directories($ENV{HOME}/github/openwrt/staging_dir/target-x86_64_musl/usr/lib/)

file(GLOB restconf_SRC
        "src/*.h"
        "src/*.c"
        "src/generated/*.h"
        "src/generated/*.c"
        "src/uci/*.h"
        "src/uci/*.c"
        "src/yang-verify.h"
        "src/yang-verify.c"
        )

add_executable(restconf
        ${restconf_SRC} src/restconf-verify.c src/restconf-verify.h)

target_link_libraries(restconf json-c uci ubox)