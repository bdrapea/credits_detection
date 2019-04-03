cmake_minimum_required(VERSION 2.8)
project(credits_detection)
set(SRCS "src/main.cpp"
         "src/credits_detection.cpp"
         "src/crde_exception.cpp")
set(HDRS "src/credits_detection.h"
         "src/crde_exception.h")

#Link OpenCV lib
find_package(OpenCV)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV required: Please install the library")
endif()

#link boost lib
find_package(Boost)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "Boost required: Please install the library")
endif()

#Set the final executable
include_directories(${OpenCV_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SRCS} ${HDRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS}
                                      ${Boost_LIBS} -lboost_system -lboost_filesystem)
