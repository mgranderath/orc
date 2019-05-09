cmake_minimum_required(VERSION 3.13)
project(restconf)

set(CMAKE_CXX_STANDARD 14)

if (NOT DEFINED ENV{STAGING_DIR})
    message(FATAL_ERROR "STAGING_DIR environment variable not set")
endif ()

if (NOT DEFINED ENV{OPENWRT_COMPILER})
    message(FATAL_ERROR "OPENWRT_COMPILER environment variable not set")
endif ()

set(CMAKE_C_COMPILER $ENV{OPENWRT_COMPILER})

include_directories(src)
include_directories($ENV{STAGING_DIR}/usr/include/)
link_directories($ENV{STAGING_DIR}/usr/lib/)

file(GLOB restconf_SRC
        "src/*.h"
        "src/*.c"
        "src/generated/*.h"
        "src/generated/*.c"
        "src/uci/*.h"
        "src/uci/*.c"
        )

add_executable(restconf ${restconf_SRC})
target_link_libraries(restconf json-c uci ubox)