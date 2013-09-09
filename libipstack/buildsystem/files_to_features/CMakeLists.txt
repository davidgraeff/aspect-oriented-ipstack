# Build tool
# For graphically adding files to features
cmake_minimum_required (VERSION 2.8.8)
project(files_to_features)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

if (Qt5Widgets_FOUND)
	include_directories(${Qt5Widgets_INCLUDES})
	add_definitions(${Qt5Widgets_DEFINITIONS})

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} -std=c++11")

	add_executable(${PROJECT_NAME} main.cpp componentModel.cpp fileModel.cpp mainwindow.cpp
	pickdependencies.cpp dependencyModel.cpp)
	target_link_libraries(${PROJECT_NAME} ${Qt5Widgets_LIBRARIES})
	
	SET(FILES_TO_FEATURES_TOOL_FOUND 1 CACHE INTERNAL)
endif()