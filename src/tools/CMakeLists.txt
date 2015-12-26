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
