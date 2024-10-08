# CMake module to search for the C library libyaml
#
# If the library is found, then yaml_FOUND is set to TRUE,
# and following variables are set:
#    yaml_INCLUDE_DIR
#    yaml_LIBRARIES

find_path(YAML_INCLUDE_DIR NAMES yaml.h)
find_library(YAML_LIBRARIES NAMES yaml libyaml)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAML DEFAULT_MSG YAML_LIBRARIES YAML_INCLUDE_DIR)
mark_as_advanced(yaml_INCLUDE_DIR YAML_LIBRARIES)

