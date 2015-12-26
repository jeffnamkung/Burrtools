find_package(FLTK REQUIRED)
find_package(PNG REQUIRED)

add_definitions(${PNG_DEFINITIONS})

include_directories(${FLTK_INCLUDE_DIR})

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
target_link_libraries(help_lib ${FLTK_LIBRARIES})
target_link_libraries(help_lib ${PNG_LIBRARIES})
