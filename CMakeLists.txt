cmake_minimum_required (VERSION 2.8.8)
project (aspectipstack)

# build kconfig
add_subdirectory (kconfig-frontends)

# build ipstack static lib
add_subdirectory (libipstack)

# build integrations
add_subdirectory (integration)