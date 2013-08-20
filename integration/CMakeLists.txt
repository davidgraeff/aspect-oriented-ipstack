cmake_minimum_required (VERSION 2.8.8)
project (aspectipstackintegration)

option (BUILD_CHIBIOS  "Build Chibi-os integration" OFF) 
option (BUILD_CIAOOS  "Build CiAO-os integration" OFF) 
option (BUILD_LINUXUSERSPACE_WITH_ASPECTS  "Build linux userspace integration (with aspects)" ON) 
option (BUILD_LINUXUSERSPACE_WITHOUT_ASPECTS  "Build linux userspace integration (without aspects)" ON) 
option (BUILD_LINUXUSERSPACE_WITHOUT_MULTITASK  "Build linux userspace integration (without aspects or multitask)" ON) 

include(examples/examples.cmake)

# using aspects and multitask
if (BUILD_CHIBIOS)
	add_subdirectory (chibi_os)
endif()
if (BUILD_CIAOOS)
	add_subdirectory (ciao_os)
endif()

# linux userspace examples
if (BUILD_LINUXUSERSPACE_WITH_ASPECTS)
	add_subdirectory (linux_userspace_with_aspects)
endif()
if (BUILD_LINUXUSERSPACE_WITHOUT_ASPECTS)
	add_subdirectory (linux_userspace_without_aspects)
endif()
if (BUILD_LINUXUSERSPACE_WITHOUT_MULTITASK)
	add_subdirectory (linux_userspace_without_aspects_multitask)
endif()