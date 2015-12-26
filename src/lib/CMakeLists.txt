set(SOURCES
#    src/lib/tabs_0/generator_0.cpp
#    src/lib/tabs_0/generator_mesh_0.cpp
#    src/lib/tabs_1/generator_1.cpp
#    src/lib/tabs_1/generator_mesh_1.cpp
#    src/lib/tabs_2/generator_2.cpp
#    src/lib/tabs_3/generator_mesh_3.cpp
#    src/lib/tabs_4/generator_mesh_4.cpp
assembler.cpp
assembler.h
assembler_0.cpp
assembler_0.h
assembler_1.cpp
assembler_1.h
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
disassembler.h
disassembler_0.cpp
disassembler_0.h
disassembler_a.cpp
disassembler_a.h
disassemblerhashes.cpp
disassemblerhashes.h
disassemblernode.cpp
disassemblernode.h
disassembly.cpp
disassembly.h
gridtype.cpp
gridtype.h
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
voxel_0.cpp
voxel_0.h
voxel_1.cpp
voxel_1.h
voxel_2.cpp
voxel_2.h
voxel_2_mesh.cpp
voxel_3.cpp
voxel_3.h
voxel_4.cpp
voxel_4.h
voxeltable.cpp
voxeltable.h
    )

add_library(burr_lib STATIC ${SOURCES})
