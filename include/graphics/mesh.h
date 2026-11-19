#ifndef MY_3D_WORLD_MESH_H
#define MY_3D_WORLD_MESH_H
#include "glad/glad.h"
#include <stdlib.h>

typedef struct {
    float position[3];
    float normal[3];
    float tex_coords[2];
} Vertex;

typedef struct {
    GLuint vao, vbo, ebo;
    GLsizei index_count;
} Mesh;

Mesh *mesh_new(const Vertex *vertices, size_t vertex_count, const unsigned int *indices, size_t index_count);

void mesh_draw(const Mesh *mesh);

void mesh_destroy(Mesh *mesh);

#endif //MY_3D_WORLD_MESH_H
