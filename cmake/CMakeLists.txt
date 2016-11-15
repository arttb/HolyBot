cmake_minimum_required(VERSION 2.8)

project (HolyBot)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE HolyBot_SOURCES "../src/*.cpp")
file(GLOB_RECURSE HolyBot_HEADERS "../src/*.hpp")

set (HolyBot_INCLUDE_DIRS "")
foreach (_headerFile ${HolyBot_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND HolyBot_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES HolyBot_INCLUDE_DIRS)

include_directories(${HolyBot_INCLUDE_DIRS})
add_executable (../bin/HolyBot ${HolyBot_SOURCES})

target_link_libraries(../bin/HolyBot ${OpenCV_LIBS})
