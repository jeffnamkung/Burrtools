cmake_minimum_required(VERSION 3.4)
project(burrtools_0_6_3)

#set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS
             thread
             filesystem
             iostreams
             date_time
             system
             REQUIRED)
find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(PNG REQUIRED)

add_definitions(-DBOOST_ALL_DYN_LINK
                -DBOOST_THREAD_USE_LIB
                ${PNG_DEFINITIONS})

include_directories(${FLTK_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(src)

set(SOURCE_FILES
    src/config.h
#    src/burrTxt.cpp
#    src/burrTxt2.cpp
    )

add_executable(burrtools_0_6_3 ${SOURCE_FILES})

target_link_libraries(burrtools_0_6_3 flu_lib)
target_link_libraries(burrtools_0_6_3 gui_lib)
target_link_libraries(burrtools_0_6_3 halfedge_lib)
target_link_libraries(burrtools_0_6_3 help_lib)
target_link_libraries(burrtools_0_6_3 burr_lib)
target_link_libraries(burrtools_0_6_3 lua_lib)
target_link_libraries(burrtools_0_6_3 tools_lib)

target_link_libraries(burrtools_0_6_3 ${FLTK_LIBRARIES})
target_link_libraries(burrtools_0_6_3 ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(burrtools_0_6_3 ${OPENGL_LIBRARIES})
target_link_libraries(burrtools_0_6_3 ${Boost_LIBRARIES})
target_link_libraries(burrtools_0_6_3 ${PNG_LIBRARIES})
