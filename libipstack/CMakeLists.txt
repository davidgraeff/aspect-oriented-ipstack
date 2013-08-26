# Look at doc/cmake_integrate_libipstack.mk for futher help

cmake_minimum_required (VERSION 2.8.8)

############## BUILD_ONLY_ONE_TASK ##############
# * Remove all busy waiting
# * Add ipstack_periodic()
# * You have to use ipstack_isreachable(addr) before sending
option (BUILD_ONLY_ONE_TASK  "Assume we have only one main loop and no multitask os" OFF) 

# Add files
if (BUILD_ONLY_ONE_TASK)
	SET(IPSTACK_ADDITIONAL_SRC ${IPSTACK_ADDITIONAL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/src/no_multitask/no_multitask.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/no_multitask/no_multitask.ah" "${CMAKE_CURRENT_SOURCE_DIR}/src/no_multitask/no_multitask.h")
endif()

############## BUILD_WITH_ASPECTLESS_INTERFACE ##############
# * Add api calls for putting received network traffic into the ipstack
#   and a function pointer that is called by the ipstack for sending packets.
option (BUILD_WITH_ASPECTLESS_INTERFACE  "Build the library and provide methods for channeling network traffic in to / out of the ipstack" OFF) 

# Add files
if (BUILD_WITH_ASPECTLESS_INTERFACE)
	SET(IPSTACK_ADDITIONAL_SRC ${IPSTACK_ADDITIONAL_SRC} "${CMAKE_CURRENT_SOURCE_DIR}/src/aspectless_external_interface/receive_and_send.ah")
endif()

############## KCONFIG ##############
# * You do not have to use kconfig in that case the entire feature set is build.
#   Selecting or deselecting features works by adding/removing files ;)
# * The buildsystem tries to setup a working kconfig frontend for you. kconfig
#   is recommended.

IF (NOT BUILD_LIBIPSTACK_WITHOUT_KCONFIG)
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

	file(READ "${KCONFIG_PRESET_CONFIGURATION_FILE}" TEMP)
	file(WRITE ${KCONFIG_RESULT_FILE} ${TEMP})

	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${KCONFIG_RESULT_FILE}")

	############## KCONFIG Reconfigure ##############
	add_custom_target(reconfigure
		COMMAND ${CMAKE_COMMAND}
		-DKCONFIG_EXECUTABLE=${KCONFIG_EXECUTABLE} -DKCONFIG_FEATUREMODEL=${KCONFIG_FEATUREMODEL}
		-DKCONFIG_RESULT_FILE_DIR=${KCONFIG_RESULT_FILE_DIR}
		-P "${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/kconfig.cmake"
		DEPENDS "${KCONFIG_EXECUTABLE}" COMMENT "Executing kconfig" VERBATIM)

	if (NOT EXISTS "${KCONFIG_RESULT_FILE}")
		add_custom_command(OUTPUT "${KCONFIG_RESULT_FILE}" DEPENDS reconfigure)
		add_custom_target(reconfigure_firsttime DEPENDS reconfigure)
	else()
		add_custom_target(reconfigure_firsttime)
	endif()
endif(NOT BUILD_LIBIPSTACK_WITHOUT_KCONFIG)

############## Source FILES ##############
# Determine relevant files based on the features selected

add_subdirectory(buildsystem/determine_relevant_files)
SET(SRC_FILE "${CMAKE_BINARY_DIR}/relevant_source_files")
SET(DEFINITIONS_FILE "${CMAKE_BINARY_DIR}/kconfig_definitions")
add_custom_command(OUTPUT "${SRC_FILE}"
	COMMAND ${CMAKE_COMMAND}
	-Doutput_filename=${SRC_FILE} -Doutput_definitions_filename=${DEFINITIONS_FILE}
	-DfeaturesFilesRelationFile=${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/ipstack.json
	-Dbase_directory=${CMAKE_CURRENT_SOURCE_DIR}/src
	-DkconfigFile=${KCONFIG_RESULT_FILE} -Dall_features=${BUILD_LIBIPSTACK_WITHOUT_KCONFIG}
	-P "${CMAKE_CURRENT_SOURCE_DIR}/buildsystem/determine_relevant_files.cmake"
	DEPENDS determine_relevant_files reconfigure_firsttime COMMENT "Executing relevant_source_files" VERBATIM)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${SRC_FILE}")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${SRC_FILE}.bak")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DEFINITIONS_FILE}")

############## Build ##############
# Write all variables of cmake to a file that is read in by the buildlib.cmake script
set(CacheForScript "${CMAKE_CURRENT_BINARY_DIR}/CMakeCacheForScript.cmake")
file(WRITE ${CacheForScript} "")
message("write to ${CacheForScript}")

get_cmake_property(Vars VARIABLES)
foreach(Var ${Vars})
  if(${Var})
    string(REPLACE "\\" "\\\\" ${Var} ${${Var}})
  endif()
  file(APPEND ${CacheForScript} "set(${Var} \"${${Var}}\")\n")
endforeach()

add_custom_target(buildlib ALL
	COMMAND ${CMAKE_COMMAND}
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	DEPENDS "${SRC_FILE}"
	)
