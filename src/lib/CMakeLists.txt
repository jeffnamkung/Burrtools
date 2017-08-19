set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS thread system REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_definitions(
    -DBOOST_ALL_DYN_LINK
    -DBOOST_THREAD_USE_LIB
)

include_directories(${Boost_INCLUDE_DIR})

set(SOURCES
    #    src/lib/tabs_0/generator_0.cpp
    #    src/lib/tabs_0/generator_mesh_0.cpp
    #    src/lib/tabs_1/generator_1.cpp
    #    src/lib/tabs_1/generator_mesh_1.cpp
    #    src/lib/tabs_2/generator_2.cpp
    #    src/lib/tabs_3/generator_mesh_3.cpp
    #    src/lib/tabs_4/generator_mesh_4.cpp
    assembler-interface.cpp
    assembler-interface.h
    don-knuth-assembler.cpp
    don-knuth-assembler.h
    wei_hwa_huang_assembler.cpp
    wei_hwa_huang_assembler.h
    assembly.cpp
    assembly.h
    bitfield.h
    bt_assert.cpp
    bt_assert.h
    burrgrower.cpp
    burrgrower.h
    converter.cpp
    converter.h
    disasmtomoves.cpp
    disasmtomoves.h
    disassembler-interface.h
    simple-disassembler.cpp
    simple-disassembler.h
    base-disassembler.cpp
    base-disassembler.h
    disassemblerhashes.cpp
    disassemblerhashes.h
    disassemblernode.cpp
    disassemblernode.h
    disassembly.cpp
    disassembly.h
    grid-type.cpp
    grid-type.h
    grouping.cpp
    grouping.h
    millable.cpp
    millable.h
    movementanalysator.cpp
    movementanalysator.h
    movementcache.cpp
    movementcache.h
    movementcache_0.cpp
    movementcache_0.h
    movementcache_1.cpp
    movementcache_1.h
    print.cpp
    print.h
    problem.cpp
    problem.h
    ps3dloader.cpp
    ps3dloader.h
    puzzle.cpp
    puzzle.h
    solution.cpp
    solution.h
    solvethread.cpp
    solvethread.h
    stl.cpp
    stl.h
    stl_0.cpp
    stl_0.h
    stl_2.cpp
    stl_2.h
    symmetries.h
    symmetries_0.cpp
    symmetries_0.h
    symmetries_1.cpp
    symmetries_1.h
    symmetries_2.cpp
    symmetries_2.h
    thread.cpp
    thread.h
    types.h
    voxel.cpp
    voxel.h
    cube_voxel.cpp
    cube_voxel.h
    triangle_voxel.cpp
    triangle_voxel.h
    sphere_voxel.cpp
    sphere_voxel.h
    voxel_2_mesh.cpp
    rhombic_voxel.cpp
    rhombic_voxel.h
    voxel_4.cpp
    voxel_4.h
    voxeltable.cpp
    voxeltable.h
    )

add_library(burr_lib STATIC ${SOURCES})
target_link_libraries(burr_lib ${Boost_LIBRARIES})
