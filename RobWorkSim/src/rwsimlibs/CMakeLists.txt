# IMPORTANT the order in which the directories are included depends on
# the individual dependencies of each lib


# the gui is a collection of generally usefull gui tools
add_subdirectory(gui)

add_subdirectory(ode)

FIND_FILE(RWPE CMakeLists.txt PATHS rwpe NO_DEFAULT_PATH)
IF(RWPE)
	ADD_SUBDIRECTORY(rwpe)
ENDIF()

add_subdirectory(bullet) 
add_subdirectory(swig)
add_subdirectory(tools)
add_subdirectory(test)

# the plugins may use all of the above 
add_subdirectory(plugins)
