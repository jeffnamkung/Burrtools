find_package(FLTK REQUIRED)

include_directories(${FLTK_INCLUDE_DIR})

set(SOURCES
    fileexists.cpp
    fileexists.h
    gzstream.cpp
    gzstream.h
    homedir.cpp
    homedir.h
    intdiv.h
    xml.cpp
    xml.h
    )

add_library(tools_lib STATIC ${SOURCES})
target_link_libraries(tools_lib ${FLTK_LIBRARIES})
