set(SOURCES
    Fl_Help_Dialog.cpp
    Fl_Help_Dialog.h
    Fl_Help_View.cpp
    Fl_Help_View.h
    flstring.cpp
    flstring.h
    helpdata.h
#    data2c.cpp
#    helpviewer.cpp
    )

add_library(help_lib STATIC ${SOURCES})
