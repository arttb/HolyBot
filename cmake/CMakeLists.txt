cmake_minimum_required(VERSION 2.8)

project (HolyBot)
find_package(OpenCV REQUIRED)

IF(APPLE)
   #SET(GUI_TYPE MACOSX_BUNDLE)
   INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(QUICKTIME_LIBRARY QuickTime )
   FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
   MARK_AS_ADVANCED (CARBON_LIBRARY
                     QUICKTIME_LIBRARY
                     APP_SERVICES_LIBRARY)
   SET(EXTRA_LIBS ${CARBON_LIBRARY} ${QUICKTIME_LIBRARY} ${APP_SERVICES_LIBRARY})
ENDIF (APPLE)

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

target_link_libraries(../bin/HolyBot ${OpenCV_LIBS} ${EXTRA_LIBS})
