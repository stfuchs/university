cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(e3_classification)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}) #"${CMAKE_SOURCE_DIR}/../MLUtils/cmake/"

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin" CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib" CACHE PATH "Build directory" FORCE)

find_package(Eigen REQUIRED)
include_directories(${EIGEN_INCLUDE_DIRS})
find_package(Boost COMPONENTS filesystem system program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}/../MLUtils/include/")


add_executable(main main.cpp)
target_link_libraries(main ${Boost_LIBRARIES})
