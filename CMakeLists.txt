cmake_minimum_required(VERSION 2.8)
project(credits_detection)

#set the c++ version to c++14
set(CMAKE_CXX_STANDARD 14)

#Link OpenCV lib
find_package(OpenCV REQUIRED)

#link boost lib
find_package(Boost REQUIRED)

#set global variables
set(SRCS "src/main.cpp"
         "src/credits_detection.cpp"
         "src/crde_exception.cpp"
         "src/crde_utils.cpp")
set(HDRS "src/credits_detection.h"
         "src/crde_exception.h"
         "src/crde_utils.h")

#Set the final executable
include_directories(${OpenCV_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SRCS} ${HDRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS}
                                      ${Boost_LIBS} -lboost_system -lboost_filesystem)
