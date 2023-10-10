include_directories(
    ${CMAKE_BINARY_DIR}
)

set(confplussrc
    conf.cpp
    backend.cpp
    exception.cpp
)

add_library(confplus SHARED ${confplussrc})

target_link_libraries(confplus dl)

install(TARGETS confplus DESTINATION lib)
