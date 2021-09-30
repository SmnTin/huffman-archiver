cmake_minimum_required(VERSION 3.13)
project(hw2)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

include_directories(lib include)
SET(SOURCE src/huffman.cpp src/codes.cpp src/tree.cpp src/long_code.cpp src/decode_table.cpp src/headers.cpp)
add_executable(hw_02 ${SOURCE} src/main.cpp)
add_executable(hw_02_test ${SOURCE} test/test.cpp test/test_huffman.cpp test/test_codes.cpp test/test_tree.cpp test/test_long_code.cpp test/test_decode_table.cpp test/test_fast_io.cpp test/test_code_io.cpp)