#include "../../include/graphics/mesh.h"

Mesh *mesh_new(const Vertex *vertices, const size_t vertex_count, const unsigned int *indices, size_t index_count) {
    Mesh *mesh = malloc(sizeof(Mesh));
    if (!mesh) {
        return NULL;
    }
    mesh->vao = mesh->vbo = mesh->ebo = 0;
    mesh->index_count = (GLsizei) index_count;

    // create id's
    glCreateVertexArrays(1, &mesh->vao);
    glCreateBuffers(1, &mesh->vbo);
    glCreateBuffers(1, &mesh->ebo);
    // fill buffers
    glNamedBufferData(mesh->vbo, (GLsizeiptr) (vertex_count * sizeof(Vertex)), vertices, GL_STATIC_DRAW);
    glNamedBufferData(mesh->ebo, (GLsizeiptr) (index_count * sizeof(unsigned int)), indices, GL_STATIC_DRAW);
    // attach to vao
    glVertexArrayVertexBuffer(mesh->vao, 0, mesh->vbo, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(mesh->vao, mesh->ebo);
    // location 0: Position (3 floats)
    glEnableVertexArrayAttrib(mesh->vao, 0);
    glVertexArrayAttribFormat(mesh->vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribBinding(mesh->vao, 0, 0);
    // location 1: Normal (3 floats)
    glEnableVertexArrayAttrib(mesh->vao, 1);
    glVertexArrayAttribFormat(mesh->vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(mesh->vao, 1, 0);
    // location 2: TexCoords (2 floats)
    glEnableVertexArrayAttrib(mesh->vao, 2);
    glVertexArrayAttribFormat(mesh->vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, tex_coords));
    glVertexArrayAttribBinding(mesh->vao, 2, 0);

    return mesh;
}

void mesh_draw(const Mesh *mesh) {
    if (!mesh || mesh->vao == 0) {
        return;
    }
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
}

void mesh_destroy(Mesh *mesh) {
    if (!mesh) {
        return;
    }
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);
    free(mesh);
}
