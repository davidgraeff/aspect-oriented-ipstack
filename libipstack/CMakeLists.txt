# Use add_subdirectory() to include the ipstack in your own CMake based application
# or operating system. You need to provide the kconfig executable either as file in
# BUILD_DIR/qconfig or via the variable KCONFIG_EXECUTABLE.
#
# Already using kconfig
# ---------------------
# If you also use kconfig to configure your os or app, you may want to include buildsystem/ipstack.fm
# into the file you handle over to kconfig. You may use the build tool in buildsystem/merge_kconfig_input.
# Set KCONFIG_PRESET_CONFIGURATION_FILE to the resulting
# kconfig output file and set the variable KCONFIG_ALREADY_DONE to 1. This will avoid calling kconfig
# a second time.
# If you want a preselection (use a kconfig output file as another input for kconfig) for the ipstack for
# your own kconfig execution, you need to add buildsystem/kconfig_output_default to your .config file before calling
# kconfig. You may use the build tool in buildsystem/merge_kconfig_output to merge your defaults with
# ipstacks defaults.
#
# Additional (aspect) files for the ipstack
# -----------------------------------------
# If you want additional files to be considered by the aspectc++ compiler for the ipstack, for example
# if you want to add aspect files to manipulate the stack, set the variable IPSTACK_ADDITIONAL_SRC.

cmake_minimum_required (VERSION 2.8.8)

# KConfig executable
if (NOT DEFINED KCONFIG_EXECUTABLE)
	SET(KCONFIG_EXECUTABLE "${CMAKE_BINARY_DIR}/qconfig")
endif()

if(NOT EXISTS "${KCONFIG_EXECUTABLE}" AND NOT DEFINED KCONFIG_ALREADY_DONE)
	message(FATAL_ERROR "KConfig has to be build first!")
endif()

# KConfig executable
if (NOT DEFINED KCONFIG_PRESET_CONFIGURATION_FILE)
	SET(KCONFIG_PRESET_CONFIGURATION_FILE "${CMAKE_SOURCE_DIR}/buildsystem/kconfig_output_default")
endif()

# Reconfigure
option (KCONFIG_RECONFIGURE "Reconfigure with kconfig" OFF)
if (KCONFIG_RECONFIGURE)
	unset(KCONFIG_ALREADY_DONE)
endif()

# Include build tool
include("buildsystem/determine_relevant_files/determine_relevant_files.cmake")

project (libipstack ACXX CXX)

# if BUILD_ONLY_ONE_TASK:
# * Remove all busy waiting
# * Add ipstack_periodic()
# * You have to use ipstack_isreachable(addr) before sending
option (BUILD_ONLY_ONE_TASK  "Assume we have only one main loop and no multitask os" OFF) 

# if BUILD_WITH_ASPECTLESS_INTERFACE:
# * Add api calls for putting received network traffic into the ipstack
#   and a function pointer that is called by the ipstack for sending packets.
option (BUILD_WITH_ASPECTLESS_INTERFACE  "Build the library and provide methods for channeling network traffic in to / out of the ipstack" OFF) 

include_directories ("${CMAKE_SOURCE_DIR}/api" "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}")

# First copy the default kconfig output file to the build dir.
# Call kconfig with ipstack.fm and kconfig_output_default
FILE(COPY "${KCONFIG_PRESET_CONFIGURATION_FILE}" DESTINATION ${CMAKE_BINARY_DIR})
if (NOT DEFINED KCONFIG_ALREADY_DONE)
	execute_process(COMMAND "${KCONFIG_EXECUTABLE}" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" RESULT_VARIABLE KCONFIG_RESULT)
	if (NOT KCONFIG_RESULT EQUAL 0) # some error occured
		message(FATAL_ERROR "KConfig did not run successfully!")
	endif()
endif()

# Determine relevant files based on the features selected
GET_RELEVANT_FILES("${KCONFIG_PRESET_CONFIGURATION_FILE}" "${CMAKE_SOURCE_DIR}/buildsystem/ipstack.json")
if(NOT EXISTS "${CMAKE_BINARY_DIR}/relevant_source_files")
	message(FATAL_ERROR "KConfig did not run successfully!")
endif()
FILE(READ "${CMAKE_BINARY_DIR}/relevant_source_files" SRC)

# Add files
if (BUILD_ONLY_ONE_TASK)
	SET(SRC ${SRC} "${CMAKE_SOURCE_DIR}/src/no_multitask/no_multitask.cpp" "${CMAKE_SOURCE_DIR}/src/no_multitask/no_multitask.ah" "${CMAKE_SOURCE_DIR}/src/no_multitask/no_multitask.h")
endif()

# Add files
if (BUILD_WITH_ASPECTLESS_INTERFACE)
	SET(SRC ${SRC} "${CMAKE_SOURCE_DIR}/src/aspectless_external_interface/receive_and_send.ah")
endif()

# optimize for size, prepare object files for the linker: remove unused methods
add_definitions(-Os -fdata-sections -ffunction-sections -ffast-math -fomit-frame-pointer -fwhole-program -fvisibility=hidden -fvisibility-inlines-hidden -fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-unwind-tables)

add_library(${PROJECT_NAME} STATIC ${SRC} ${IPSTACK_ADDITIONAL_SRC})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,--gc-sections -Wl,--strip-all)
