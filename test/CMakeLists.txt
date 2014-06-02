cmake_minimum_required(VERSION 2.8.10)

project(test C CXX)

file(GLOB CPP_FILES *.cpp)

include_directories(${ASM_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${CPP_FILES})

target_link_libraries(${PROJECT_NAME} ASM)

SET(CMAKE_CXX_FLAGS "-std=c++0x")
