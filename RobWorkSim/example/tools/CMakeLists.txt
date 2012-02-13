
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# The name of the project.
PROJECT(RWSimTools)

# Used to resolve absolute path names
SET(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_VERBOSE_MAKEFILE false)

# Now set the RW/RWS root (edit this if necessary)

SET(RW_ROOT "${ROOT}/../../../RobWork")
SET(RWS_ROOT "${ROOT}/../../../RobWorkStudio")
SET(RWSIM_ROOT "${ROOT}/../../../RobWorkSim")


# We use the settings that robwork studio uses
IF( NOT DEFINED CMAKE_BUILD_TYPE )
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()
#Include default settings for constructing a robwork dependent project
#INCLUDE(${RW_ROOT}/build/FindRobWork.cmake)
#INCLUDE_DIRECTORIES( ${ROBWORK_INCLUDE_DIR} )
#LINK_DIRECTORIES( ${ROBWORK_LIBRARY_DIRS} )

#Include default settings for constructing a robworkstudio dependent project
#INCLUDE(${RWSTUDIO_ROOT}/build/FindRobWorkStudio.cmake)
#INCLUDE_DIRECTORIES( ${ROBWORKSTUDIO_INCLUDE_DIR} )
#LINK_DIRECTORIES( ${ROBWORKSTUDIO_LIBRARY_DIRS} )

#Include RobWorkSim settings

MESSAGE(" ${RWSIM_ROOT} --")
SET(CMAKE_MODULE_PATH ${RWSIM_ROOT}/build ${CMAKE_MODULE_PATH})
FIND_PACKAGE(RobWorkSim REQUIRED)
INCLUDE_DIRECTORIES( ${ROBWORKSIM_INCLUDE_DIR} )
LINK_DIRECTORIES( ${ROBWORKSIM_LIBRARY_DIRS} )


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


#add_executable(GraspTest grasptest.cpp)
#target_link_libraries(GraspTest rwsim ${LibraryList})

#add_executable(ForceClosureTest forceclosuretest.cpp)
#target_link_libraries(ForceClosureTest rwsim ${LibraryList})

LINK_DIRECTORIES( ${BulletLinkDir} ${OdeLinkDir} )
INCLUDE_DIRECTORIES( ${BulletIncludes} ${OdeIncludes} )

INCLUDE_DIRECTORIES("${RW_ROOT}/example/simplegui")

add_executable(ModelPropertyTest src/ModelPropertyTest.cpp)
target_link_libraries(ModelPropertyTest rwsim 
    ${ROBWORK_LIBRARIES})

add_executable(RWSimTest src/RWSimTest.cpp)
target_link_libraries(RWSimTest rwsim ${ROBWORK_LIBRARIES})

#add_executable(UniformSelectGrasps UniformSelectGrasps.cpp)
#target_link_libraries(UniformSelectGrasps rwsim ${ROBWORK_LIBRARIES})

add_executable(ToGraspTaskHGP src/ToGraspTaskHGP.cpp)
target_link_libraries(ToGraspTaskHGP rwsim ${ROBWORK_LIBRARIES})

add_executable(ModelStablePoseTest src/ModelStablePoseTest.cpp)
target_link_libraries(ModelStablePoseTest rwsim ${ROBWORK_LIBRARIES})

add_executable(GraspTableConvert src/GraspTableConvert.cpp)
target_link_libraries(GraspTableConvert rwsim ${ROBWORK_LIBRARIES})

add_executable(LoadGraspTableKTH src/LoadGraspTableKTH.cpp)
target_link_libraries(LoadGraspTableKTH rwsim ${ROBWORK_LIBRARIES})

add_executable(ScaleWorkCell src/ScaleWorkCell.cpp)
target_link_libraries(ScaleWorkCell rwsim ${ROBWORK_LIBRARIES})

add_executable(PathFilter src/PathFilter.cpp)
target_link_libraries(PathFilter rwsim ${ROBWORK_LIBRARIES})

add_executable(SimulateTask src/SimulateTask.cpp)
target_link_libraries(SimulateTask rwsim ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(SimulateTaskLB src/SimulateTaskLB.cpp)
target_link_libraries(SimulateTaskLB rwsim ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(GTaskStats src/GTaskStats.cpp)
target_link_libraries(GTaskStats rwsim ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(GenSuctionCupTask src/GenSuctionCupTask.cpp)
target_link_libraries(GenSuctionCupTask rwsim ${ROBWORK_LIBRARIES})

add_executable(SampleSurface src/SampleSurface.cpp)
target_link_libraries(SampleSurface rwsim ${ROBWORK_LIBRARIES})

add_executable(ConvertSimTask src/ConvertSimTask.cpp)
target_link_libraries(ConvertSimTask rwsim ${ROBWORK_LIBRARIES})

add_executable(imageFeaturesToStl src/imageFeaturesToStl.cpp)
target_link_libraries(imageFeaturesToStl rwsim ${ROBWORK_LIBRARIES})

add_executable(ExtendVisGrabTask src/ExtendVisGrabTask.cpp)
target_link_libraries(ExtendVisGrabTask rwsim ${ROBWORK_LIBRARIES})

add_executable(gtmerge src/gtmerge.cpp)
target_link_libraries(gtmerge rwsim ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(ParJawGripperPoseSampler src/ParJawGripperPoseSampler.cpp)
target_link_libraries(ParJawGripperPoseSampler rwsim ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

add_executable(ToComma src/ToComma.cpp)
target_link_libraries(ToComma rwsim ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})

#add_executable(VisGraB VisGraB.cpp VisGraBGraspTask.cpp VisGraBBenchmark.cpp SDHInvKinSolver.cpp IKSoftCSolver.cpp)
#target_link_libraries(VisGraB rwsim ${ROBWORK_LIBRARIES} ${ROBWORKSIM_LIBRARIES})
#MESSAGE("${ROBWORK_LIBRARIES}")

# add_executable(RemoteRobworkBuilding RemoteRobworkBuilding.cpp)
# target_link_libraries(RemoteRobworkBuilding ${ROBWORK_LIBRARIES})


#add_executable(RestingPoseSimulation RestingPoseSimulation.cpp)
#target_link_libraries(RestingPoseSimulation rwsim ${ROBWORK_LIBRARIES})