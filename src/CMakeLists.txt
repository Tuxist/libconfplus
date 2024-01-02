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

install(TARGETS confplus DESTINATION lib EXPORT confplusTargets)

set(ConfigPackageLocation lib/cmake/libconfplus)

export(EXPORT confplusTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/libnetplusTargets.cmake"
  NAMESPACE Upstream::
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/libconfplusVersion.cmake"
  VERSION ${Upstream_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(EXPORT confplusTargets
  FILE
    libconfplusTargets.cmake
  NAMESPACE
    Upstream::
  DESTINATION
    ${ConfigPackageLocation}
)

install(
  FILES
    "libconfplusConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libconfplusVersion.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/libconfplusConfig.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
