cmake_minimum_required(VERSION 3.16)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(dma_heap container/test/test_container.cpp container/BT.cpp container/BT.h time_window.cpp time_window.h container/vector_heap.cpp container/vector_heap.h test_util.h container/circle_queue.h)
add_executable(cpp main.cpp container/BT.cpp container/BT.h time_window.cpp time_window.h container/vector_heap.cpp container/vector_heap.h test_util.h)
