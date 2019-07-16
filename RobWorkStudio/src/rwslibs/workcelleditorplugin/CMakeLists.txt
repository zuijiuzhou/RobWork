SET(SUBSYS_NAME rws_workcelleditorplugin)
set(SUBSYS_DESC "Workcell editor plugin")
SET(SUBSYS_DEPS rws rws_workcelleditor)

# Standard cpp files to compile:
SET(SrcFiles WorkcellEditorPlugin.cpp)
SET(SRC_FILES_HPP WorkcellEditorPlugin.hpp)

IF(RWS_USE_QT5)
QT5_WRAP_CPP(MocSrcFiles ${SRC_FILES_HPP})
    # Rcc the files:
    QT5_ADD_RESOURCES(RccSrcFiles resources.qrc)
ELSE()
# Header files to moc and compile:
    QT4_WRAP_CPP(MocSrcFiles ${SRC_FILES_HPP} OPTIONS -DBOOST_TT_HAS_PLUS_HPP_INCLUDED -DBOOST_TT_HAS_PLUS_ASSIGN_HPP_INCLUDED -DBOOST_TT_HAS_MINUS_HPP_INCLUDED -DBOOST_TT_HAS_MINUS_ASSIGN_HPP_INCLUDED)
    # Rcc the files:
    QT4_ADD_RESOURCES(RccSrcFiles resources.qrc)
ENDIF()

# Set extra linker flags for apple macos computers
RWS_ADD_PLUGIN(${SUBSYS_NAME} plugins ${RWS_DEFAULT_LIB_TYPE} ${SrcFiles} ${MocSrcFiles} ${RccSrcFiles})
RW_ADD_INCLUDES(plugins "rwslibs/workcelleditorplugin" ${SRC_FILES_HPP})
TARGET_LINK_LIBRARIES(${SUBSYS_NAME} ${SUBSYS_DEPS})

IF( "${RWS_DEFAULT_LIB_TYPE}" STREQUAL "STATIC" )
set(RWS_PLUGIN_LIBRARIES ${RWS_PLUGIN_LIBRARIES} ${SUBSYS_NAME} PARENT_SCOPE)
ENDIF()