cmake_minimum_required (VERSION 2.8.8)
project (libipstack)

# if BUILD_ONLY_ONE_TASK:
# * Remove all busy waiting
# * Add ipstack_periodic()
# * You have to use ipstack_isreachable(addr) before sending
option (BUILD_ONLY_ONE_TASK  "Assume we have only one main loop and no multitask os" OFF) 

include_directories ("${CMAKE_SOURCE_DIR}/api" "${CMAKE_SOURCE_DIR}/src")

if (BUILD_ONLY_ONE_TASK)
	SET(SRC ${SRC} "${CMAKE_SOURCE_DIR}/api/no_multitask.cpp")
else()

endif()

if (BUILD_ONLY_LIB)

else()

endif()

add_library(${PROJECT_NAME} STATIC ${SRC})
#target_link_libraries (${PROJECT_NAME} ${EXTRA_LIBS})