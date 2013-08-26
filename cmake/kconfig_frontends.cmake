enable_language(CXX)
# Find Qt because we want it for kconfigs qconfig
find_package (Qt COMPONENTS QtCore REQUIRED)
find_program(autoconf_prog autoconf)
if (NOT autoconf_prog)
	message(FATAL_ERROR "Autoconf is neccessary for kconfig-frontends")
endif()
unset(autoconf_prog)
find_program(autoconf_prog automake)
if (NOT autoconf_prog)
	message(FATAL_ERROR "Automake is neccessary for kconfig-frontends")
endif()
unset(autoconf_prog)
find_program(autoconf_prog libtoolize)
if (NOT autoconf_prog)
	message(FATAL_ERROR "libtoolize is neccessary for kconfig-frontends")
endif()
unset(autoconf_prog)
find_program(autoconf_prog gperf)
if (NOT autoconf_prog)
	message(FATAL_ERROR "gperf is neccessary for kconfig-frontends")
endif()
unset(autoconf_prog)
find_program(autoconf_prog flex)
find_program(autoconf_prog2 lex)
if (NOT autoconf_prog AND NOT autoconf_prog2)
	message(FATAL_ERROR "flex or lex is neccessary for kconfig-frontends")
endif()
unset(autoconf_prog)
unset(autoconf_prog2)
message(STATUS "Download and build kconfig")
# We download and build in BUILD_DIR/kconfig-frontends but install to
# the source directory for futur use
SET(BUILD_KCONFIG_DIR "${CMAKE_BINARY_DIR}/kconfig-frontends-build")
SET(INSTALL_KCONFIG_DIR "${CMAKE_BINARY_DIR}/kconfig-frontends")
include(ExternalProject)
ExternalProject_Add(KCONFIG PREFIX "kconfig-frontends-build" TMP_DIR ${BUILD_KCONFIG_DIR}/tmp
	STAMP_DIR ${BUILD_KCONFIG_DIR}/stamp DOWNLOAD_DIR ${BUILD_KCONFIG_DIR}/download
	SOURCE_DIR ${BUILD_KCONFIG_DIR}/download
	GIT_REPOSITORY git://ymorin.is-a-geek.org/kconfig-frontends
	GIT_TAG origin/master
	CONFIGURE_COMMAND ${BUILD_KCONFIG_DIR}/download/configure --prefix=${INSTALL_KCONFIG_DIR} > /dev/null
	BUILD_IN_SOURCE 1
	BUILD_COMMAND make -j2 > /dev/null INSTALL_COMMAND make install > /dev/null)
ExternalProject_Add_Step(KCONFIG CONFIGURE2
	COMMAND ${BUILD_KCONFIG_DIR}/download/bootstrap > /dev/null
	COMMENT "Bootstrap"
	WORKING_DIRECTORY ${BUILD_KCONFIG_DIR}/download
	DEPENDERS configure DEPENDEES download)

SET(KCONFIG_EXECUTABLE "${INSTALL_KCONFIG_DIR}/bin/kconfig-qconf" CACHE STRING ""  FORCE)