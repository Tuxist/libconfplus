project(libconfplus)
cmake_minimum_required(VERSION 3.00)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -fPIC \
    -Wall"
)

configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)

set(LIBV "1")
set(Upstream_VERSION 1.0.0)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(backends)
add_subdirectory(tools)
