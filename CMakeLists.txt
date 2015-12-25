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

set(SOURCE_FILES
    src/flu/Flu_Button.cpp
    src/flu/Flu_Button.h
    src/flu/Flu_Combo_Box.cpp
    src/flu/Flu_Combo_Box.h
    src/flu/Flu_Combo_List.cpp
    src/flu/Flu_Combo_List.h
    src/flu/Flu_Combo_Tree.cpp
    src/flu/Flu_Combo_Tree.h
    src/flu/Flu_Enumerations.h
    src/flu/flu_export.h
    src/flu/Flu_File_Chooser.cpp
    src/flu/Flu_File_Chooser.h
    src/flu/flu_file_chooser_pixmaps.cpp
    src/flu/flu_file_chooser_pixmaps.h
    src/flu/Flu_Label.cpp
    src/flu/Flu_Label.h
    src/flu/flu_pixmaps.cpp
    src/flu/flu_pixmaps.h
    src/flu/Flu_Return_Button.cpp
    src/flu/Flu_Return_Button.h
    src/flu/Flu_Separator.cpp
    src/flu/Flu_Separator.h
    src/flu/Flu_Tree_Browser.cpp
    src/flu/Flu_Tree_Browser.h
    src/flu/Flu_Wrap_Group.cpp
    src/flu/Flu_Wrap_Group.h
    src/flu/FluSimpleString.cpp
    src/flu/FluSimpleString.h
    src/flu/FluVectorClass.h
    src/gui/arcball.cpp
    src/gui/arcball.h
    src/gui/assertwindow.cpp
    src/gui/assertwindow.h
    src/gui/assmimportwindow.cpp
    src/gui/assmimportwindow.h
    src/gui/BlockList.cpp
    src/gui/BlockList.h
    src/gui/blocklistgroup.cpp
    src/gui/blocklistgroup.h
    src/gui/buttongroup.cpp
    src/gui/buttongroup.h
    src/gui/configuration.cpp
    src/gui/configuration.h
    src/gui/constraintsgroup.cpp
    src/gui/constraintsgroup.h
    src/gui/convertwindow.cpp
    src/gui/convertwindow.h
    src/gui/Fl_Table.cpp
    src/gui/Fl_Table.h
    src/gui/gl2ps.c
    src/gui/gl2ps.h
    src/gui/grideditor.cpp
    src/gui/grideditor.h
    src/gui/grideditor_0.cpp
    src/gui/grideditor_0.h
    src/gui/grideditor_1.cpp
    src/gui/grideditor_1.h
    src/gui/grideditor_2.cpp
    src/gui/grideditor_2.h
    src/gui/grideditor_3.cpp
    src/gui/grideditor_3.h
    src/gui/grideditor_4.cpp
    src/gui/grideditor_4.h
    src/gui/gridtypegui.cpp
    src/gui/gridtypegui.h
    src/gui/groupseditor.cpp
    src/gui/groupseditor.h
    src/gui/guigridtype.cpp
    src/gui/guigridtype.h
    src/gui/image.cpp
    src/gui/image.h
    src/gui/imageexport.cpp
    src/gui/imageexport.h
    src/gui/Images.cpp
    src/gui/Images.h
    src/gui/Layouter.cpp
    src/gui/Layouter.h
    src/gui/LFl_Tile.cpp
    src/gui/LFl_Tile.h
    src/gui/main.cpp
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/gui/movementbrowser.cpp
    src/gui/movementbrowser.h
    src/gui/multilinewindow.cpp
    src/gui/multilinewindow.h
    src/gui/piececolor.cpp
    src/gui/piececolor.h
    src/gui/placementbrowser.cpp
    src/gui/placementbrowser.h
    src/gui/resultviewer.cpp
    src/gui/resultviewer.h
    src/gui/separator.cpp
    src/gui/separator.h
    src/gui/statusline.cpp
    src/gui/statusline.h
    src/gui/statuswindow.cpp
    src/gui/statuswindow.h
    src/gui/stlexport.cpp
    src/gui/stlexport.h
    src/gui/togglebutton.cpp
    src/gui/togglebutton.h
    src/gui/tooltabs.cpp
    src/gui/tooltabs.h
    src/gui/tr.c
    src/gui/tr.h
    src/gui/vectorexportwindow.cpp
    src/gui/vectorexportwindow.h
    src/gui/view3dgroup.cpp
    src/gui/view3dgroup.h
    src/gui/voxeleditgroup.cpp
    src/gui/voxeleditgroup.h
    src/gui/voxelframe.cpp
    src/gui/voxelframe.h
    src/gui/WindowWidgets.h
    src/halfedge/face.cpp
    src/halfedge/face.h
    src/halfedge/halfedge.cpp
    src/halfedge/halfedge.h
    src/halfedge/modifiers.cpp
    src/halfedge/modifiers.h
    src/halfedge/polyhedron.cpp
    src/halfedge/polyhedron.h
    src/halfedge/polyhedron_iterators.h
    src/halfedge/vector3.cpp
    src/halfedge/vector3.h
    src/halfedge/vertex.cpp
    src/halfedge/vertex.h
    src/halfedge/volume.cpp
    src/halfedge/volume.h
#    src/help/data2c.cpp
    src/help/Fl_Help_Dialog.cpp
    src/help/Fl_Help_Dialog.h
    src/help/Fl_Help_View.cpp
    src/help/Fl_Help_View.h
    src/help/flstring.cpp
    src/help/flstring.h
    src/help/helpdata.h
#    src/help/helpviewer.cpp
#    src/lib/tabs_0/generator_0.cpp
#    src/lib/tabs_0/generator_mesh_0.cpp
#    src/lib/tabs_1/generator_1.cpp
#    src/lib/tabs_1/generator_mesh_1.cpp
#    src/lib/tabs_2/generator_2.cpp
#    src/lib/tabs_3/generator_mesh_3.cpp
#    src/lib/tabs_4/generator_mesh_4.cpp
    src/lib/assembler.cpp
    src/lib/assembler.h
    src/lib/assembler_0.cpp
    src/lib/assembler_0.h
    src/lib/assembler_1.cpp
    src/lib/assembler_1.h
    src/lib/assembly.cpp
    src/lib/assembly.h
    src/lib/bitfield.h
    src/lib/bt_assert.cpp
    src/lib/bt_assert.h
    src/lib/burrgrower.cpp
    src/lib/burrgrower.h
    src/lib/converter.cpp
    src/lib/converter.h
    src/lib/disasmtomoves.cpp
    src/lib/disasmtomoves.h
    src/lib/disassembler.h
    src/lib/disassembler_0.cpp
    src/lib/disassembler_0.h
    src/lib/disassembler_a.cpp
    src/lib/disassembler_a.h
    src/lib/disassemblerhashes.cpp
    src/lib/disassemblerhashes.h
    src/lib/disassemblernode.cpp
    src/lib/disassemblernode.h
    src/lib/disassembly.cpp
    src/lib/disassembly.h
    src/lib/gridtype.cpp
    src/lib/gridtype.h
    src/lib/grouping.cpp
    src/lib/grouping.h
    src/lib/millable.cpp
    src/lib/millable.h
    src/lib/movementanalysator.cpp
    src/lib/movementanalysator.h
    src/lib/movementcache.cpp
    src/lib/movementcache.h
    src/lib/movementcache_0.cpp
    src/lib/movementcache_0.h
    src/lib/movementcache_1.cpp
    src/lib/movementcache_1.h
    src/lib/print.cpp
    src/lib/print.h
    src/lib/problem.cpp
    src/lib/problem.h
    src/lib/ps3dloader.cpp
    src/lib/ps3dloader.h
    src/lib/puzzle.cpp
    src/lib/puzzle.h
    src/lib/solution.cpp
    src/lib/solution.h
    src/lib/solvethread.cpp
    src/lib/solvethread.h
    src/lib/stl.cpp
    src/lib/stl.h
    src/lib/stl_0.cpp
    src/lib/stl_0.h
    src/lib/stl_2.cpp
    src/lib/stl_2.h
    src/lib/symmetries.h
    src/lib/symmetries_0.cpp
    src/lib/symmetries_0.h
    src/lib/symmetries_1.cpp
    src/lib/symmetries_1.h
    src/lib/symmetries_2.cpp
    src/lib/symmetries_2.h
    src/lib/thread.cpp
    src/lib/thread.h
    src/lib/types.h
    src/lib/voxel.cpp
    src/lib/voxel.h
    src/lib/voxel_0.cpp
    src/lib/voxel_0.h
    src/lib/voxel_1.cpp
    src/lib/voxel_1.h
    src/lib/voxel_2.cpp
    src/lib/voxel_2.h
    src/lib/voxel_2_mesh.cpp
    src/lib/voxel_3.cpp
    src/lib/voxel_3.h
    src/lib/voxel_4.cpp
    src/lib/voxel_4.h
    src/lib/voxeltable.cpp
    src/lib/voxeltable.h
    src/lua/lapi.c
    src/lua/lapi.h
    src/lua/lauxlib.c
    src/lua/lauxlib.h
    src/lua/lcode.c
    src/lua/lcode.h
    src/lua/ldebug.c
    src/lua/ldebug.h
    src/lua/ldo.c
    src/lua/ldo.h
    src/lua/ldump.c
    src/lua/lfunc.c
    src/lua/lfunc.h
    src/lua/lgc.c
    src/lua/lgc.h
    src/lua/llex.c
    src/lua/llex.h
    src/lua/llimits.h
    src/lua/lmem.c
    src/lua/lmem.h
    src/lua/lobject.c
    src/lua/lobject.h
    src/lua/lopcodes.c
    src/lua/lopcodes.h
    src/lua/lparser.c
    src/lua/lparser.h
    src/lua/lstate.c
    src/lua/lstate.h
    src/lua/lstring.c
    src/lua/lstring.h
    src/lua/ltable.c
    src/lua/ltable.h
    src/lua/ltm.c
    src/lua/ltm.h
    src/lua/lua.h
    src/lua/luaclass.cpp
    src/lua/luaclass.h
    src/lua/luaconf.h
    src/lua/lundump.c
    src/lua/lundump.h
    src/lua/lvm.c
    src/lua/lvm.h
    src/lua/lzio.c
    src/lua/lzio.h
    src/tools/fileexists.cpp
    src/tools/fileexists.h
    src/tools/gzstream.cpp
    src/tools/gzstream.h
    src/tools/homedir.cpp
    src/tools/homedir.h
    src/tools/intdiv.h
    src/tools/xml.cpp
    src/tools/xml.h
    src/config.h
#    src/burrTxt.cpp
#    src/burrTxt2.cpp
    )

add_executable(burrtools_0_6_3 ${SOURCE_FILES})

target_link_libraries(burrtools_0_6_3 ${FLTK_LIBRARIES})
target_link_libraries(burrtools_0_6_3 ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(burrtools_0_6_3 ${OPENGL_LIBRARIES})
target_link_libraries(burrtools_0_6_3 ${Boost_LIBRARIES})
target_link_libraries(burrtools_0_6_3 ${PNG_LIBRARIES})
