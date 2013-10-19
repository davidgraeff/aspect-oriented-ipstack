cmake_minimum_required (VERSION 2.8.8)
project (aspectipstackintegration)

option (BUILD_CHIBIOS  "Build Chibi-os integration" OFF) 
option (BUILD_CIAOOS  "Build CiAO-os integration" OFF) 
option (BUILD_LINUXUSERSPACE  "Build linux userspace integration (singletask)" ON) 

include(examples/examples.cmake)

if (BUILD_CHIBIOS)
	add_subdirectory (chibi_os)
endif()
if (BUILD_CIAOOS)
	add_subdirectory (ciao_os)
endif()
if (BUILD_LINUXUSERSPACE)
	add_subdirectory (linux_userspace)
endif()