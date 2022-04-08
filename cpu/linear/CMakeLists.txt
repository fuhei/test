# CMakeLists.txt of CPU linear test
# Rong Tao <rongtao@cestc.cn>
# Create at 2022-04-08
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(cpu-linear)

AUX_SOURCE_DIRECTORY(src SRC_LIST)

include_directories(src)

find_library(PTHREAD pthread /usr/lib64)
find_library(NUMA numa /usr/lib64)
link_libraries(${PTHREAD} ${NUMA})

add_definitions(-MD -Wall -Werror -g -ggdb)

ADD_EXECUTABLE(linear ${SRC_LIST})