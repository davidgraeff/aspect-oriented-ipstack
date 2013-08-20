cmake_minimum_required (VERSION 2.8.8)
project (libipstack)

# Build tool
# Input: kconfig output file and feature-to-file-relation file
# Output: Selected files in file relevant_source_files.
function(GET_RELEVANT_FILES kconfigFile featuresFilesRelationFile)
	try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR
          "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/determine_relevant_files/main.cpp" CMAKE_FLAGS INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/determine_relevant_files"
          RUN_OUTPUT_VARIABLE run
          ARGS "-o" "${CMAKE_BINARY_DIR}/relevant_source_files" "-f" "${featuresFilesRelationFile}" "-k" "${kconfigFile}")
endfunction()