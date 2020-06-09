# ##################################################################################################
# Add a library target. _name The library name. _component The part of RW that this library belongs
# to. ARGN The source files for the library.
macro(RWS_ADD_PLUGIN _name _lib_type)
    add_library(${_name} ${_lib_type} ${ARGN})
    add_dependencies(${_name} sdurws)
    # Only link if needed
    if(WIN32 AND MSVC)
        set_target_properties(${_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set_target_properties(${_name} PROPERTIES LINK_FLAGS -Wl)
    elseif(__COMPILER_PATHSCALE)
        set_target_properties(${_name} PROPERTIES LINK_FLAGS -mp)
    else()
        set_target_properties(${_name} PROPERTIES LINK_FLAGS -Wl,--as-needed,--no-undefined)
    endif()

    if(${_lib_type} STREQUAL "MODULE" OR ${_lib_type} STREQUAL "SHARED")
        set_target_properties(${_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/plugins)
    endif()

    # Set the VERSION and SOVERSION of the library to the RobWorkStudio major and minor versions On
    # MAC OS we can not do this if we are building a Module (where it does not make much sense
    # anyway)
    if(NOT ("${_lib_type}" STREQUAL "MODULE" AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))
        string(REGEX MATCHALL "[0-9]+" VERSIONS ${ROBWORKSTUDIO_VERSION})
        list(GET VERSIONS 0 ROBWORKSTUDIO_VERSION_MAJOR)
        list(GET VERSIONS 1 ROBWORKSTUDIO_VERSION_MINOR)
        list(GET VERSIONS 2 ROBWORKSTUDIO_VERSION_PATCH)

        set_target_properties(
            ${_name}
            PROPERTIES VERSION ${ROBWORKSTUDIO_VERSION} SOVERSION
                       ${ROBWORKSTUDIO_VERSION_MAJOR}.${ROBWORKSTUDIO_VERSION_MINOR}
                       # DEFINE_SYMBOL "RWAPI_EXPORTS"
        )
    endif()

    install(
        TARGETS ${_name}
        EXPORT ${PROJECT_PREFIX}Targets
        RUNTIME DESTINATION ${BIN_INSTALL_DIR} COMPONENT ${_component}
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}/RobWork/rwsplugins" COMPONENT ${_component}
        ARCHIVE DESTINATION "${LIB_INSTALL_DIR}/RobWork/rwsplugins" COMPONENT ${_component}
    )

endmacro()

# ##################################################################################################
# Add a library target. _name The library name. _component The part of RW that this library belongs
# to. ARGN The source files for the library.
macro(RWS_ADD_COMPONENT _name)
    add_library(${_name} ${PROJECT_LIB_TYPE} ${ARGN})

    # Only link if needed
    if(WIN32 AND MSVC)
        set_target_properties(${_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set_target_properties(${_name} PROPERTIES LINK_FLAGS -Wl)
    elseif(__COMPILER_PATHSCALE)
        set_target_properties(${_name} PROPERTIES LINK_FLAGS -mp)
    else()
        set_target_properties(${_name} PROPERTIES LINK_FLAGS -Wl,--as-needed,--no-undefined)
    endif()
    #
    string(REGEX MATCHALL "[0-9]+" VERSIONS ${ROBWORKSTUDIO_VERSION})
    list(GET VERSIONS 0 PROJECT_VERSION_MAJOR)
    list(GET VERSIONS 1 PROJECT_VERSION_MINOR)
    list(GET VERSIONS 2 PROJECT_VERSION_PATCH)

    if(${PROJECT_USE_SONAME})
        set_target_properties(
            ${_name} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION
                                ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )
    endif()
    # if(USE_PROJECT_FOLDERS) set_target_properties(${_name} PROPERTIES FOLDER "Libraries")
    # endif(USE_PROJECT_FOLDERS)

    install(
        TARGETS ${_name}
        EXPORT ${PROJECT_PREFIX}Targets
        RUNTIME DESTINATION ${BIN_INSTALL_DIR} COMPONENT ${_name}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${_name}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${_name}
    )

endmacro()

macro(RWS_PLUGIN_LOAD_DETAILS _subsys_name _dockarea _name _visible)
    set(${_name}_DOCKAREA
        "${_name}\\DockArea=${_dockarea}"
        CACHE INTERNAL "PLugin Load details for {_name}" FORCE
    )
    set(${_name}_FILENAME
        "${_name}\\Filename=$<TARGET_FILE_NAME:${_subsys_name}>"
        CACHE INTERNAL "PLugin Load details for {_name}" FORCE
    )
    set(${_name}_PATH
        "${_name}\\Path=$<TARGET_FILE_DIR:${_subsys_name}>"
        CACHE INTERNAL "PLugin Load details for {_name}" FORCE
    )
    set(${_name}_VISIBLE
        "${_name}\\Visible=${_visible}"
        CACHE INTERNAL "PLugin Load details for {_name}" FORCE
    )
endmacro()
