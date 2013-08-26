cmake_minimum_required (VERSION 2.8.8)
project (aspectipstack NONE)

############## KCONFIG ##############
# * You do not have to use kconfig in that case the entire feature set is build.
#   Selecting or deselecting features works by adding/removing files ;)
# * The buildsystem tries to setup a working kconfig frontend for you. kconfig
#   is recommended.
# * You may provide an own kconfig executable with KCONFIG_EXECUTABLE.
option (BUILD_LIBIPSTACK_WITHOUT_KCONFIG  "Build ipstack without kconfig" OFF) 

# download,configure and build kconfig
if (NOT BUILD_LIBIPSTACK_WITHOUT_KCONFIG AND NOT EXISTS "${KCONFIG_EXECUTABLE}")
	include(cmake/kconfig_frontends.cmake)
endif()

# build ipstack static lib
add_subdirectory (libipstack)

# build integrations
add_subdirectory (integration)