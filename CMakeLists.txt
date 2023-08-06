cmake_minimum_required(VERSION 3.0)

project(volk CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(spdlog_DIR ${CMAKE_BINARY_DIR}/../include)
set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR}/include/)

set(CMAKE_CXX_STANDARD 20)

find_package(spdlog REQUIRED)

add_library(spdlog SHARED IMPORTED)

file(GLOB_RECURSE ASTERI_SRC_FILES CONFIGURE_DEPENDS
    "src/*/*.cpp"
)
add_executable(volk src/main.cpp ${ASTERI_SRC_FILES})

target_link_libraries(volk spdlog::spdlog)

target_compile_options(volk PRIVATE -g)

install(TARGETS volk RUNTIME DESTINATION bin)
