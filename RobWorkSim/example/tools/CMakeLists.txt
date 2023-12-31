#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.10  )

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

# The name of the project.
PROJECT(RWSimTools)

# Used to resolve absolute path names
SET(ROOT ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_VERBOSE_MAKEFILE false)

# Now set the RW/RWSIM root (edit this if necessary)
SET(RW_ROOT "${ROOT}/../../../RobWork")
SET(RWSIM_ROOT "${ROOT}/../../../RobWorkSim")

# We use the settings that robwork studio uses
IF( NOT DEFINED CMAKE_BUILD_TYPE )
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

#Use RobWork and RobWorkSim
SET(RobWork_DIR ${RW_ROOT}/cmake) 
SET(RobWorkSim_DIR ${RWSIM_ROOT}/cmake)
FIND_PACKAGE(RobWork REQUIRED)
FIND_PACKAGE(RobWorkSim REQUIRED)

INCLUDE_DIRECTORIES( ${ROBWORK_INCLUDE_DIRS} ${ROBWORKSIM_INCLUDE_DIRS} )

#Include RobWorkSim settings
SET(CMAKE_MODULE_PATH ${RW_ROOT}/build ${RWSIM_ROOT}/build ${CMAKE_MODULE_PATH})

# Set the output dir for generated libraries and binaries
IF(MSVC)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT}/bin" CACHE PATH "Runtime directory" FORCE)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ROOT}/libs" CACHE PATH "Library directory" FORCE)
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ROOT}/libs" CACHE PATH "Archive directory" FORCE)
ELSE()
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT}/bin/${CMAKE_BUILD_TYPE}" CACHE PATH "Runtime directory" FORCE)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ROOT}/libs/${CMAKE_BUILD_TYPE}" CACHE PATH "Library directory" FORCE)
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ROOT}/libs/${CMAKE_BUILD_TYPE}" CACHE PATH "Archive directory" FORCE)
ENDIF()

LINK_DIRECTORIES( ${BulletLinkDir} ${OdeLinkDir} )
INCLUDE_DIRECTORIES( ${BulletIncludes} ${OdeIncludes} )

INCLUDE_DIRECTORIES("${RW_ROOT}/example/simplegui")

add_executable(ModelPropertyTest src/ModelPropertyTest.cpp)
target_link_libraries(ModelPropertyTest ${ROBWORK_LIBRARIES})

add_executable(RWSimTest src/RWSimTest.cpp)
target_link_libraries(RWSimTest ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(UniformSelectGrasps src/UniformSelectGrasps.cpp)
target_link_libraries(UniformSelectGrasps ${ROBWORK_LIBRARIES})

add_executable(ToGraspTaskHGP src/ToGraspTaskHGP.cpp)
target_link_libraries(ToGraspTaskHGP ${ROBWORK_LIBRARIES})

add_executable(ModelStablePoseTest src/ModelStablePoseTest.cpp)
target_link_libraries(ModelStablePoseTest ${ROBWORK_LIBRARIES})

add_executable(GraspTableConvert src/GraspTableConvert.cpp)
target_link_libraries(GraspTableConvert ${ROBWORK_LIBRARIES})

add_executable(LoadGraspTableKTH src/LoadGraspTableKTH.cpp)
target_link_libraries(LoadGraspTableKTH ${ROBWORK_LIBRARIES})

add_executable(LPEPoseQualityCalc src/LPEPoseQualityCalc.cpp)
target_link_libraries(LPEPoseQualityCalc ${ROBWORK_LIBRARIES})

add_executable(FindRegionSE3 src/FindRegionSE3.cpp)
target_link_libraries(FindRegionSE3 ${ROBWORK_LIBRARIES})

add_executable(PathFilter src/PathFilter.cpp)
target_link_libraries(PathFilter ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(SimulateTask src/SimulateTask.cpp)
target_link_libraries(SimulateTask ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(SimulateTaskLB src/SimulateTaskLB.cpp)
target_link_libraries(SimulateTaskLB ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(GTaskStats src/GTaskStats.cpp)
target_link_libraries(GTaskStats ${ROBWORK_LIBRARIES})

add_executable(GenSuctionCupTask src/GenSuctionCupTask.cpp)
target_link_libraries(GenSuctionCupTask ${ROBWORK_LIBRARIES})

add_executable(SampleSurface src/SampleSurface.cpp)
target_link_libraries(SampleSurface ${ROBWORK_LIBRARIES})

add_executable(ConvertSimTask src/ConvertSimTask.cpp)
target_link_libraries(ConvertSimTask ${ROBWORK_LIBRARIES})

add_executable(imageFeaturesToStl src/imageFeaturesToStl.cpp)
target_link_libraries(imageFeaturesToStl ${ROBWORK_LIBRARIES})

add_executable(ExtendVisGrabTask src/ExtendVisGrabTask.cpp)
target_link_libraries(ExtendVisGrabTask ${ROBWORK_LIBRARIES})

add_executable(GraspTaskQualityCalc src/GraspTaskQualityCalc.cpp)
target_link_libraries(GraspTaskQualityCalc ${ROBWORK_LIBRARIES})

add_executable(gtmerge src/gtmerge.cpp)
target_link_libraries(gtmerge ${ROBWORK_LIBRARIES})

add_executable(ParJawGripperPoseSampler src/ParJawGripperPoseSampler.cpp)
target_link_libraries(ParJawGripperPoseSampler ${ROBWORK_LIBRARIES})

add_executable(ToComma src/ToComma.cpp)
target_link_libraries(ToComma ${ROBWORK_LIBRARIES})

add_executable(danniToGtable src/danniToGtable.cpp)
target_link_libraries(danniToGtable ${ROBWORK_LIBRARIES})

add_executable(cut src/cut.cpp src/CutActionParam.cpp)
target_link_libraries(cut ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(DBCutFiltering src/DBCutFiltering.cpp src/CutActionParam.cpp)
target_link_libraries(DBCutFiltering ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(SimulateAssembly src/SimulateAssembly.cpp)
target_link_libraries(SimulateAssembly ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(GenerateCircularPiHTasks src/GenerateCircularPiHTasks.cpp)
target_link_libraries(GenerateCircularPiHTasks ${ROBWORK_LIBRARIES})
