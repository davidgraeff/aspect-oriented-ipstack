# Look at doc/cmake_integrate_libipstack.mk for futher help

cmake_minimum_required (VERSION 2.8.8)

include(${CMAKE_BINARY_DIR}/CMakeCacheForScript.cmake)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

############## Aspect compiler ##############
# Find and check ACXX compiler
if(NOT CMAKE_ACXX_COMPILER_WORKS)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../buildsystem/acxx")
	enable_language(ACXX)
endif()
if(NOT CMAKE_ACXX_COMPILER_WORKS)
  message(FATAL_ERROR "Required working ACXX compiler not found.")
endif()

############## Project and Compiler ##############
project (libipstack ACXX)
SET(CMAKE_GENERIC_ACXX_FLAGS "${CMAKE_GENERIC_ACXX_FLAGS} --real-instances --no_warn_macro --keep_woven -k -p${CMAKE_CURRENT_SOURCE_DIR}")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../api" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")

if (NOT EXISTS "${SRC_FILE}")
	message(FATAL_ERROR "No SRC_FILE variable provided! The file contains the source files depending on the selected features")
endif()

FILE(READ "${SRC_FILE}" SRC)

if (NOT EXISTS "${DEFINITIONS_FILE}")
	message(FATAL_ERROR "No DEFINITIONS_FILE variable provided! The file contains the kconfig key-value data as compiler definitions")
endif()

FILE(READ "${DEFINITIONS_FILE}" SRC_DEFINITIONS)

# Check the DEBUG Environment variable for debugging build
IF(($ENV{DEBUG} STREQUAL "1") OR (DEFINED IPSTACK_DEBUG))
  SET(IPSTACK_DEBUG 1 CACHE INTERNAL "Build ${CMAKE_CURRENT_SOURCE_DIR} in debug mode")
  MESSAGE(STATUS "Build IPStack in debug mode")
  add_definitions( -ggdb -fno-omit-frame-pointer -fno-function-sections -DDEBUG ${SRC_DEFINITIONS})
ELSE()
	# optimize for size, prepare object files for the linker: remove unused methods
	add_definitions(-Os -fdata-sections -ffunction-sections -ffast-math -fomit-frame-pointer -fwhole-program -fvisibility=hidden -fvisibility-inlines-hidden -fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-unwind-tables ${SRC_DEFINITIONS})
ENDIF()

add_library(${PROJECT_NAME} STATIC ${SRC} ${IPSTACK_ADDITIONAL_SRC})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,--gc-sections -Wl,--strip-all")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)