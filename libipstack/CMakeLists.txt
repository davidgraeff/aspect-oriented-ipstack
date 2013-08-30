# Look at doc/cmake_integrate_libipstack.mk for futher help

cmake_minimum_required (VERSION 2.8.8)

############## BUILD_ONLY_ONE_TASK ##############
# * Remove all busy waiting
# * Add ipstack_periodic()
# * You have to use ipstack_isreachable(addr) before sending
option (BUILD_ONLY_ONE_TASK  "Assume we have only one main loop and no multitask os" OFF) 

# Add files
if (BUILD_ONLY_ONE_TASK)
	SET(IPSTACK_ADDITIONAL_SRC ${IPSTACK_ADDITIONAL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/src/no_multitask/no_multitask.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/no_multitask/no_multitask.ah" "${CMAKE_CURRENT_SOURCE_DIR}/api/no_multitask.h")
endif()

############## BUILD_WITH_ASPECTLESS_INTERFACE ##############
# * Add api calls for putting received network traffic into the ipstack
#   and a function pointer that is called by the ipstack for sending packets.
option (BUILD_WITH_ASPECTLESS_INTERFACE  "Build the library and provide methods for channeling network traffic in to / out of the ipstack" OFF) 

# Add files
if (BUILD_WITH_ASPECTLESS_INTERFACE)
	SET(IPSTACK_ADDITIONAL_SRC ${IPSTACK_ADDITIONAL_SRC}
		"${CMAKE_CURRENT_SOURCE_DIR}/src/aspectless_external_interface/receive_and_send.ah"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/aspectless_external_interface/os_integration.ah"
		"${CMAKE_CURRENT_SOURCE_DIR}/api/receive_and_send.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/api/os_integration.h")
endif()

############## BUILD_LIBIPSTACK_WITH_ALL_FILES ##############
# Ony do this for continous integration!
option (BUILD_LIBIPSTACK_WITH_ALL_FILES "Ony do this for continous integration!" OFF) 

############## BUILD_WITH_ASPECTLESS_INTERFACE and not BUILD_ONLY_ONE_TASK ##############
# irq safeness for aspectless interface with multiple tasks
if (BUILD_WITH_ASPECTLESS_INTERFACE AND NOT BUILD_ONLY_ONE_TASK)
	SET(IPSTACK_ADDITIONAL_SRC ${IPSTACK_ADDITIONAL_SRC}
		"${CMAKE_CURRENT_SOURCE_DIR}/src/aspectless_external_interface/lock_multitask_access.ah"
		"${CMAKE_CURRENT_SOURCE_DIR}/api/lock_multitask_access.h")
endif()

############## KCONFIG and determine_relevant_files ##############
# * You do not have to use kconfig in that case a predefined feature set is build.
#   Selecting or deselecting features works by editing the buildsystem/kconfig_output_default file ;)
# * The buildsystem tries to setup a working kconfig frontend for you. kconfig
#   is recommended.

if (NOT DEFINED KCONFIG_RESULT_FILE_DIR)
	SET(KCONFIG_RESULT_FILE_DIR "${CMAKE_BINARY_DIR}")
endif()
SET(KCONFIG_RESULT_FILE "${KCONFIG_RESULT_FILE_DIR}/.config")

if (NOT DEFINED KCONFIG_FEATUREMODEL)
	SET(KCONFIG_FEATUREMODEL "${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/ipstack.fm")
endif()

# First copy the default kconfig output file to the build dir.
if (NOT DEFINED KCONFIG_PRESET_CONFIGURATION_FILE)
	SET(KCONFIG_PRESET_CONFIGURATION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/kconfig_output_default")
endif()

SET(SRC_FILE "${CMAKE_BINARY_DIR}/relevant_source_files")
SET(DEFINITIONS_FILE "${CMAKE_BINARY_DIR}/cfAttribs.h") # Get configuration variables from kconfig and store in cfAttribs.h
add_subdirectory(buildsystem/determine_relevant_files)
SET(CLEAN_FILES "${SRC_FILE}" "${SRC_FILE}.bak" "${DEFINITIONS_FILE}")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_FILES}")

add_custom_target(clean-configuration COMMAND ${CMAKE_COMMAND} -E remove "${KCONFIG_RESULT_FILE}")

IF (NOT BUILD_LIBIPSTACK_WITHOUT_KCONFIG)
	############## KCONFIG Reconfigure and determine_relevant_files ##############
	# Determine relevant files based on the features selected
	add_custom_target(reconfigure
		COMMAND ${CMAKE_COMMAND}
		-DKCONFIG_EXECUTABLE=${KCONFIG_EXECUTABLE} -DKCONFIG_FEATUREMODEL=${KCONFIG_FEATUREMODEL}
		-DKCONFIG_RESULT_FILE_DIR=${KCONFIG_RESULT_FILE_DIR}
		-DKCONFIG_RESULT_FILE=${KCONFIG_RESULT_FILE}
		-DKCONFIG_PRESET_CONFIGURATION_FILE=${KCONFIG_PRESET_CONFIGURATION_FILE}
		-P "${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/kconfig.cmake"
		COMMAND ${CMAKE_COMMAND}
		-Doutput_filename=${SRC_FILE} -Doutput_definitions_filename=${DEFINITIONS_FILE}
		-DfeaturesFilesRelationFile=${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/ipstack.json
		-Dbase_directory=${CMAKE_CURRENT_SOURCE_DIR}/src
		-DkconfigFile=${KCONFIG_RESULT_FILE} -Dall_features=${BUILD_LIBIPSTACK_WITHOUT_KCONFIG}
		-P "${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/determine_relevant_files.cmake"
		COMMAND cmake -E touch "${CMAKE_SOURCE_DIR}/CMakeLists.txt"
		DEPENDS "${KCONFIG_EXECUTABLE}" COMMENT "Executing kconfig+determine_relevant_files"
		DEPENDS determine_relevant_files VERBATIM)
else()
	# no kconfig -> use predefined KCONFIG_RESULT_FILE
	file(READ "${KCONFIG_PRESET_CONFIGURATION_FILE}" TEMP)
	file(WRITE ${KCONFIG_RESULT_FILE} ${TEMP})
	
	############## determine_relevant_files ##############
	# Determine relevant files based on the features selected
	add_custom_target(reconfigure
		COMMAND ${CMAKE_COMMAND}
		-Doutput_filename=${SRC_FILE} -Doutput_definitions_filename=${DEFINITIONS_FILE}
		-DfeaturesFilesRelationFile=${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/ipstack.json
		-Dbase_directory=${CMAKE_CURRENT_SOURCE_DIR}/src
		-DkconfigFile=${KCONFIG_RESULT_FILE} -Dall_files=${BUILD_LIBIPSTACK_WITH_ALL_FILES}
		-P "${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/determine_relevant_files.cmake"
		COMMAND cmake -E touch "${CMAKE_SOURCE_DIR}/CMakeLists.txt"
		DEPENDS determine_relevant_files COMMENT "Executing relevant_source_files" VERBATIM)
endif()

############## Build or rerun cmake ##############
if (NOT EXISTS "${SRC_FILE}" OR NOT EXISTS "${DEFINITIONS_FILE}")
	# File that lists the relevant source files is not present. We have to rerun cmake.
	message(STATUS "In this cmake run, only configuration takes place. cmake will automatically rerun for building.")
	add_custom_target(rerun ALL DEPENDS reconfigure COMMENT "Run again for building!")
	#message(FATAL_ERROR "No SRC_FILE variable provided! The file contains the source files depending on the selected features")
else()
	SET(IPSTACK_ADDITIONAL_SRC ${IPSTACK_ADDITIONAL_SRC} "${DEFINITIONS_FILE}") 
	add_subdirectory(src)
endif()


