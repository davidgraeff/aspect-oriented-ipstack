cmake_minimum_required (VERSION 2.8.8)
project(linux_userspace_ipstack)

option (BUILD_LINUXUSERSPACE_WITHOUT_ASPECTS  "Build linux userspace integration without aspects" OFF) 
option (BUILD_LINUXUSERSPACE_SINGLETASK  "Build linux userspace integration as singletask application" OFF)

# Add sources (including the selected example source
SET(SRC_CXX_FILES "tapdev.cpp" ${EXAMPLE_SRC} "main.cpp")

IF (BUILD_LINUXUSERSPACE_SINGLETASK) # singletask src
	SET(SRC_CXX_FILES ${SRC_CXX_FILES} "main_singletask.cpp")
ELSE() # multitask src
	SET(SRC_CXX_FILES ${SRC_CXX_FILES} "main.cpp")
ENDIF()

IF (BUILD_LINUXUSERSPACE_WITHOUT_ASPECTS) # build without aspects
	file(GLOB SRC_CXX_ADD_FILES "without_aspects/*.cpp")
	SET(SRC_CXX_FILES ${SRC_CXX_FILES} ${SRC_CXX_ADD_FILES})
	
	# no additional sources for the ipstack
	SET(BUILD_WITH_ASPECTLESS_INTERFACE TRUE CACHE INTERNAL "")
	SET(IPSTACK_ADDITIONAL_SRC "" CACHE INTERNAL "")
	
	# singletask src
	file(GLOB SRC_CXX_ADD_FILES "without_aspects/singletask/*.cpp")
	SET(SRC_CXX_FILES ${SRC_CXX_FILES} ${SRC_CXX_ADD_FILES})
	SET(BUILD_ONLY_ONE_TASK TRUE "" CACHE INTERNAL "")
ELSE() # build with aspects
	file(GLOB SRC_AH_FILES "with_aspects/*.ah")
	
	# our aspect files are additional sources for the ipstack
	SET(BUILD_WITH_ASPECTLESS_INTERFACE FALSE CACHE INTERNAL "")
	SET(IPSTACK_ADDITIONAL_SRC ${SRC_AH_FILES} CACHE INTERNAL "")
	
	IF (BUILD_LINUXUSERSPACE_SINGLETASK) # singletask src
		file(GLOB SRC_CXX_ADD_FILES "with_aspects/singletask/*.cpp")
		SET(SRC_CXX_FILES ${SRC_CXX_FILES} ${SRC_CXX_ADD_FILES})
		SET(BUILD_ONLY_ONE_TASK TRUE "" CACHE INTERNAL "")
	ELSE() # multitask src
		file(GLOB SRC_CXX_ADD_FILES "with_aspects/multitask/*.cpp")
		SET(SRC_CXX_FILES ${SRC_CXX_FILES} ${SRC_CXX_ADD_FILES})
		SET(BUILD_ONLY_ONE_TASK FALSE "" CACHE INTERNAL "")
	ENDIF()
ENDIF()

include_directories("${CMAKE_SOURCE_DIR}/libipstack/src" "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${PROJECT_NAME}  ${SRC_CXX_FILES} DEPENDS libipstack)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,--gc-sections -Wl,--strip-all")