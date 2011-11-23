cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(testing)

find_package( OpenCV REQUIRED )
#include_directories(${OpenCV_INCLUDE_DIRS})
#link_directories(${OpenCV_LIBRARY_DIRS})
#add_definitions(${OpenCV_DEFINITIONS})

add_executable(testing testing.cpp)
target_link_libraries(testing ${OpenCV_LIBS})
