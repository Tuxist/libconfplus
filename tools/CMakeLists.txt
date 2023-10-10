
include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(confctl confctl.cpp)
target_link_libraries(confctl confplus)

install(TARGETS confctl DESTINATION bin)
