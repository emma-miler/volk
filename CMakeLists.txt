cmake_minimum_required(VERSION 3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)


project(volk)

add_executable(volk src/main.cpp)

install(TARGETS volk RUNTIME DESTINATION bin)
