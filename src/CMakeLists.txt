set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

configure_file(config.h.in config.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(flu)
add_subdirectory(gui)
add_subdirectory(halfedge)
add_subdirectory(help)
add_subdirectory(lib)
add_subdirectory(lua)
add_subdirectory(tools)

set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS thread system REQUIRED)
find_package(Threads REQUIRED)

add_definitions(
    -DBOOST_ALL_DYN_LINK
    -DBOOST_THREAD_USE_LIB
)

include_directories(${Boost_INCLUDE_DIR})

add_executable(burrGui config.h)
target_link_libraries(burrGui
    flu_lib
    gui_lib
    halfedge_lib
    help_lib
    burr_lib
    lua_lib
    tools_lib
    )

add_executable(burrTxt burrTxt.cpp)
target_link_libraries(burrTxt
    flu_lib
    halfedge_lib
    help_lib
    burr_lib
    lua_lib
    tools_lib
    ${Boost_LIBRARIES}
    )

add_executable(burrTxt2 burrTxt2.cpp)
target_link_libraries(burrTxt2
    flu_lib
    halfedge_lib
    help_lib
    burr_lib
    lua_lib
    tools_lib
    ${Boost_LIBRARIES}
    )

