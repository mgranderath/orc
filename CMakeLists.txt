cmake_minimum_required(VERSION 3.11.1)
project(restconf)

set(CMAKE_CXX_STANDARD 14)
include_directories(src)

find_path(JSON_C_INCLUDE_DIR json-c/json.h)
find_path(UCI_INCLUDE_DIR uci/uci.h)
include_directories(${JSON_C_INCLUDE_DIR})
include_directories(${UCI_INCLUDE_DIR})

file(GLOB_RECURSE restconf_SRC "src/*.h" "src/*.c")

find_library(JSON_C json-c)
find_library(UCI uci)
find_library(UBOX ubox)

add_executable(restconf ${restconf_SRC})
target_link_libraries(restconf ${JSON_C} ${UCI} ${UBOX})
INSTALL(TARGETS restconf RUNTIME DESTINATION /www/cgi-bin/)