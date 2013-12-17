# Look at doc/cmake_integrate_libipstack.mk for futher help
# This cmake file works only if the aspect compiler has been discovered before
# and if "${CMAKE_CURRENT_SOURCE_DIR}/../buildsystem/filterfiles.cmake" can be found

cmake_minimum_required (VERSION 2.8.8)
include("${CMAKE_CURRENT_SOURCE_DIR}/../buildsystem/filterfiles.cmake")

############## Project and Compiler ##############
project (libipstack ACXX CXX)

#### compiler setup ####
ADD_DEFINITIONS_ACXX(--real-instances --no_warn_macro --keep_woven -k --no_line -p${CMAKE_CURRENT_SOURCE_DIR})
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../api" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")
# Check the DEBUG Environment variable for debugging build
IF(($ENV{DEBUG} STREQUAL "1") OR (DEFINED IPSTACK_DEBUG))
	SET(IPSTACK_DEBUG 1 CACHE INTERNAL "Build ${CMAKE_CURRENT_SOURCE_DIR} in debug mode")
	MESSAGE(STATUS "Build IPStack in debug mode")
	add_definitions( -ggdb -fno-omit-frame-pointer -fno-function-sections -DDEBUG)
ELSE()
	# optimize for size, prepare object files for the linker: remove unused methods
	add_definitions(-Os -fdata-sections -ffunction-sections -ffast-math -fomit-frame-pointer -fwhole-program -fvisibility=hidden -fvisibility-inlines-hidden -fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-unwind-tables)
ENDIF()

#### Read file that contains a list of all relevant source files
FILE(READ "${SRC_FILE}" SRC)
SET(SRC ${IPSTACK_ADDITIONAL_SRC} ${SRC})
if (IPSTACK_ADDITIONAL_SRC)
	message("libipstack: Additional files: ${IPSTACK_ADDITIONAL_SRC}")
endif()

# Compile all cpp sources with ACXX
SET(SRC_CXX_FILES)
SET(SRC_AH_FILES)
filter_files_two(SRC_CXX_FILES ".cpp" SRC_AH_FILES ".ah" ${SRC})
set_source_files_properties(${SRC_CXX_FILES} PROPERTIES LANGUAGE ACXX)

# Workaround for ac++. Only take .ah files into consideration which are relevant.
# We use the -a argument for this.
foreach(AH_FILE ${SRC_AH_FILES})
	STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" SMALLFILE "${AH_FILE}")
	ADD_DEFINITIONS_ACXX("-a" "${AH_FILE}")
	#message("ah file ${SMALLFILE}")
endforeach()

add_library(${PROJECT_NAME} STATIC ${SRC_CXX_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,--gc-sections -Wl,--strip-all")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE ACXX)

# Extracting additional flags
# foreach(SRC_file ${SRC_CXX_FILES})
#   ACXX_AUTOMATIC_FLAGS(${SRC_file})
#   ACXX_AUTOMATIC_DEPENDENCIES(${SRC_file})
# endforeach()
