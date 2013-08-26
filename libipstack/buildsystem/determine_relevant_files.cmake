# Build tool
# Input: kconfig output file and feature-to-file-relation file
# Output: Selected files in file relevant_source_files.

# MESSAGE(STATUS "Run determine_relevant_files")
SET(EXEC "${CMAKE_CURRENT_BINARY_DIR}/buildsystem/determine_relevant_files/determine_relevant_files")
SET(ARGUMENTS "${EXEC}" "-b" "${base_directory}" "-o" "${output_filename}"
	"-d" "${output_definitions_filename}" "-f" "${featuresFilesRelationFile}")

if (all_features) # select all files
	LIST(APPEND ARGUMENTS "-a")
else()
	LIST(APPEND ARGUMENTS "-k" "${kconfigFile}")
endif()

execute_process(COMMAND ${ARGUMENTS} RESULT_VARIABLE EXEC_RESULT WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if (NOT EXEC_RESULT EQUAL 0) # some error occured
	FILE(RENAME "${output_filename}" "${output_filename}.bak")
	message(FATAL_ERROR "determine_relevant_files failed: ${EXEC_RESULT}!\nArguments:" ${ARGUMENTS})
endif()
