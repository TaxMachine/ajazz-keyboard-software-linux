cmake_minimum_required(VERSION 3.30)
project(ak820proLED)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES src/**)

add_executable(ak820proLED ${SOURCES})

target_link_libraries(ak820proLED PRIVATE usb-1.0)