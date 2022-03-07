#
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

PROJECT(attach_process)

AUX_SOURCE_DIRECTORY(src SRC_LIST)
AUX_SOURCE_DIRECTORY(src/elf_utils/ SRC_LIST)
AUX_SOURCE_DIRECTORY(src/ftrace/ SRC_LIST)
AUX_SOURCE_DIRECTORY(src/file_utils/ SRC_LIST)
AUX_SOURCE_DIRECTORY(src/common_utils/ SRC_LIST)
AUX_SOURCE_DIRECTORY(src/vm_utils/ SRC_LIST)
AUX_SOURCE_DIRECTORY(src/process/ SRC_LIST)
AUX_SOURCE_DIRECTORY(src/syscalls/ SRC_LIST)

include_directories(src
    src/common_utils
    src/elf_utils
    src/ftrace
    src/file_utils
    src/vm_utils
    src/process
    src/syscalls)

find_library(RT rt /usr/lib64)
link_libraries(${RT})

add_definitions(-MD -Wall -Werror -g -ggdb)

ADD_EXECUTABLE(pattach ${SRC_LIST})