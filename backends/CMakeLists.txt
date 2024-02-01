include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

find_package(YAML REQUIRED)

if(${YAML_FOUND})
    set( yamlsrc
        yaml/yaml.cpp
    )

    add_library(yamlconf SHARED ${yamlsrc})

    set_target_properties(yamlconf PROPERTIES PREFIX "")
    set_target_properties(yamlconf PROPERTIES OUTPUT_NAME yaml)

    target_include_directories(yamlconf PUBLIC ${YAML_INCLUDE_DIRS})
    target_link_libraries(yamlconf ${YAML_LIBRARIES} confplus dl)

    install(TARGETS yamlconf DESTINATION lib/confplus/backend)

endif()

find_package(INIPARSER REQUIRED)
if(${INIPARSER_FOUND})
    set( inisrc
        ini/ini.cpp
    )

    add_library(iniconf SHARED ${inisrc})

    set_target_properties(iniconf PROPERTIES PREFIX "")
    set_target_properties(iniconf PROPERTIES OUTPUT_NAME ini)

    target_include_directories(iniconf PUBLIC ${YAML_INCLUDE_DIRS})
    target_link_libraries(iniconf ${YAML_LIBRARIES} confplus dl)

    install(TARGETS iniconf DESTINATION lib/confplus/backend)
endif()
