cmake_minimum_required(VERSION 3.30)
project(ak820proLED)
set(CMAKE_CXX_STANDARD 23)

if(WIN32)
    message(FATAL_ERROR "Invalid platform, this is a linux only project")
endif ()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

file(GLOB_RECURSE SOURCES src/**)
file(GLOB_RECURSE IMGUI_SOURCES imgui/src/** imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_glfw.cpp)
file(GLOB_RECURSE LODEPNG_SOURCES lodepng/src/**)

include_directories(imgui/include lodepng/include ${GTK_INCLUDE_DIRS})

add_executable(ak820proLED ${SOURCES} ${IMGUI_SOURCES} ${LODEPNG_SOURCES})

target_link_libraries(ak820proLED PRIVATE hidapi-libusb OpenGL::GL glfw ${GTK_LIBRARIES})