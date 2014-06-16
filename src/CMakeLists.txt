cmake_minimum_required(VERSION 2.8.0)

project(ASM)

#TODO add c++11 flag
file(GLOB CPP_FILES *.cpp libASM/*.cpp libASM/Private/*.cpp)

add_library(${PROJECT_NAME} SHARED ${CPP_FILES})

target_link_libraries(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES  VERSION 2.0.0  SOVERSION 2)

include_directories(${PROJECT_SOURCE_DIR}/libASM)
SET(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/libASM
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

SET(CMAKE_CXX_FLAGS "-std=c++0x")

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES libASM/clsASM.h DESTINATION include/lib${PROJECT_NAME})
