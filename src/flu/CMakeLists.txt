find_package(FLTK REQUIRED)

include_directories(${FLTK_INCLUDE_DIR})

set(SOURCES
    Flu_Button.cpp
    Flu_Button.h
    Flu_Combo_Box.cpp
    Flu_Combo_Box.h
    Flu_Combo_List.cpp
    Flu_Combo_List.h
    Flu_Combo_Tree.cpp
    Flu_Combo_Tree.h
    Flu_Enumerations.h
    flu_export.h
    Flu_File_Chooser.cpp
    Flu_File_Chooser.h
    flu_file_chooser_pixmaps.cpp
    flu_file_chooser_pixmaps.h
    Flu_Label.cpp
    Flu_Label.h
    flu_pixmaps.cpp
    flu_pixmaps.h
    Flu_Return_Button.cpp
    Flu_Return_Button.h
    Flu_Separator.cpp
    Flu_Separator.h
    Flu_Tree_Browser.cpp
    Flu_Tree_Browser.h
    Flu_Wrap_Group.cpp
    Flu_Wrap_Group.h
    FluSimpleString.cpp
    FluSimpleString.h
    FluVectorClass.h)

add_library(flu_lib STATIC ${SOURCES})
target_link_libraries(flu_lib ${FLTK_LIBRARIES})
