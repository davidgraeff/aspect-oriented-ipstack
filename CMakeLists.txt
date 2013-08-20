cmake_minimum_required (VERSION 2.8.8)
project (aspectipstack)

# if BUILD_ONLY_LIB:
# * Add api calls for putting received network traffic into the ipstack
#   and a function pointer that is called by the ipstack for sending packets.
option (BUILD_ONLY_LIB  "Use as external static library" OFF) 

add_subdirectory (libipstack)

if (BUILD_ONLY_LIB)

else()
	add_subdirectory (integration)
endif()