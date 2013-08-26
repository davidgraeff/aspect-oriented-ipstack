cmake_minimum_required (VERSION 2.8.8)

############## KConfig ################
# KConfig executable
if(NOT DEFINED KCONFIG_EXECUTABLE OR NOT EXISTS "${KCONFIG_EXECUTABLE}")
	message(FATAL_ERROR "KConfig has to be build first!")
endif()

If (DEFINED Skip_if_existing AND EXISTS "${KCONFIG_RESULT_FILE}")
	return()
endif()

if (NOT EXISTS "${KCONFIG_RESULT_FILE}")
	file(READ "${KCONFIG_PRESET_CONFIGURATION_FILE}" TEMP)
	file(WRITE ${KCONFIG_RESULT_FILE} ${TEMP})
endif()

# Call kconfig with ipstack.fm with the working directory where .config is located
MESSAGE(STATUS "Run kconfig: ${KCONFIG_EXECUTABLE} ${KCONFIG_FEATUREMODEL}")
execute_process(COMMAND "${KCONFIG_EXECUTABLE}" "${KCONFIG_FEATUREMODEL}"
	WORKING_DIRECTORY "${KCONFIG_RESULT_FILE_DIR}" RESULT_VARIABLE KCONFIG_RESULT)

if (NOT KCONFIG_RESULT EQUAL 0) # some error occured
	FILE(REMOVE "${KCONFIG_RESULT_FILE_DIR}/.config")
	message(FATAL_ERROR "KConfig did not run successfully!")
endif()
