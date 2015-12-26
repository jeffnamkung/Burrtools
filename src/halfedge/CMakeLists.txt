set(SOURCES
    face.cpp
    face.h
    halfedge.cpp
    halfedge.h
    modifiers.cpp
    polyhedron.cpp
    polyhedron.h
    polyhedron_iterators.h
    vector3.cpp
    vector3.h
    vertex.cpp
    vertex.h
    volume.cpp
    volume.h)

add_library(halfedge_lib STATIC ${SOURCES})
