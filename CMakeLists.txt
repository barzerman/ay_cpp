cmake_minimum_required(VERSION 3.16)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(dma_heap tree/test/test_dma_heap.cpp tree/BT.cpp tree/BT.h time_window.cpp time_window.h tree/vector_heap.cpp tree/vector_heap.h test_util.h)
add_executable(cpp main.cpp tree/BT.cpp tree/BT.h time_window.cpp time_window.h tree/vector_heap.cpp tree/vector_heap.h test_util.h)
