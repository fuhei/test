#
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

PROJECT(upatch)

AUX_SOURCE_DIRECTORY(src/ SRC_LIST)

include_directories(src)

find_library(RT rt /usr/lib64)
link_libraries(${RT})

add_definitions(-MD -Wall -Werror -g -ggdb)

ADD_EXECUTABLE(upatch ${SRC_LIST} ${DIR_SRCS})