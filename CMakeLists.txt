cmake_minimum_required(VERSION 3.22)
project(standalone_osi_trace_file_player)

set(CMAKE_CXX_STANDARD 17)

add_executable(standalone_osi_trace_file_player main.cpp)

find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

target_link_libraries(standalone_osi_trace_file_player ${ZeroMQ_LIBRARY})
