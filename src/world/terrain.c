#include "../../include/world/terrain.h"
#include <stdlib.h>

float terrain_get_height(const PerlinNoise *noise, const TerrainConfig config, const float world_x,
                         const float world_z) {
    if (!noise) {
        return 0.0f;
    }

    const float n = perlin_fbm2d(
        noise,
        world_x * config.scale,
        world_z * config.scale,
        config.octaves,
        config.persistence,
        config.lacunarity
    );

    return n * config.max_height;
}

Terrain *terrain_new(const PerlinNoise *noise, const TerrainConfig config, vec3 position) {
    if (!noise || config.width < 2 || config.depth < 2) {
        return NULL;
    }

    Terrain *terrain = malloc(sizeof(Terrain));
    if (!terrain) {
        return NULL;
    }

    terrain->mesh = NULL;
    terrain->config = config;
    glm_vec3_copy(position, terrain->position);

    const size_t vertex_count = (size_t) config.width * (size_t) config.depth;
    const size_t index_count = (size_t) (config.width - 1) * (size_t) (config.depth - 1) * 6;

    Vertex *vertices = malloc(vertex_count * sizeof(Vertex));
    uint32_t *indices = malloc(index_count * sizeof(uint32_t));

    if (!vertices || !indices) {
        free(vertices);
        free(indices);
        free(terrain);
        return NULL;
    }

    // Positionen, Höhen & Texturkoordinaten berechnen
    for (int z = 0; z < config.depth; z++) {
        for (int x = 0; x < config.width; x++) {
            const size_t idx = (size_t) z * (size_t) config.width + (size_t) x;
            const float world_x = position[0] + (float) x;
            const float world_z = position[2] + (float) z;
            const float height = terrain_get_height(noise, config, world_x, world_z);

            vertices[idx].position[0] = (float) x;
            vertices[idx].position[1] = height;
            vertices[idx].position[2] = (float) z;

            vertices[idx].tex_coords[0] = (float) x / (float) (config.width - 1);
            vertices[idx].tex_coords[1] = (float) z / (float) (config.depth - 1);
        }
    }

    // Normalen aus den Höhenunterschieden der Nachbar-Vertices berechnen
    for (int z = 0; z < config.depth; z++) {
        for (int x = 0; x < config.width; x++) {
            const int x_l = (x > 0) ? x - 1 : x;
            const int x_r = (x < config.width - 1) ? x + 1 : x;
            const int z_d = (z > 0) ? z - 1 : z;
            const int z_u = (z < config.depth - 1) ? z + 1 : z;

            const float h_l = vertices[(size_t) z * config.width + x_l].position[1];
            const float h_r = vertices[(size_t) z * config.width + x_r].position[1];
            const float h_d = vertices[(size_t) z_d * config.width + x].position[1];
            const float h_u = vertices[(size_t) z_u * config.width + x].position[1];

            vec3 normal = {h_l - h_r, 2.0f, h_d - h_u};
            glm_vec3_normalize(normal);

            const size_t idx = (size_t) z * (size_t) config.width + (size_t) x;
            vertices[idx].normal[0] = normal[0];
            vertices[idx].normal[1] = normal[1];
            vertices[idx].normal[2] = normal[2];
        }
    }

    // Indices generieren (2 Dreiecke pro Gitterzelle in CCW-Reihenfolge)
    size_t i = 0;
    for (int z = 0; z < config.depth - 1; z++) {
        for (int x = 0; x < config.width - 1; x++) {
            const uint32_t top_left = (uint32_t) (z * config.width + x);
            const uint32_t top_right = top_left + 1;
            const uint32_t bottom_left = (uint32_t) ((z + 1) * config.width + x);
            const uint32_t bottom_right = bottom_left + 1;

            indices[i++] = top_left;
            indices[i++] = bottom_left;
            indices[i++] = top_right;

            indices[i++] = top_right;
            indices[i++] = bottom_left;
            indices[i++] = bottom_right;
        }
    }

    terrain->mesh = mesh_new(vertices, vertex_count, indices, index_count);

    free(vertices);
    free(indices);

    if (!terrain->mesh) {
        free(terrain);
        return NULL;
    }

    return terrain;
}

void terrain_destroy(Terrain *terrain) {
    if (!terrain) {
        return;
    }
    if (terrain->mesh) {
        mesh_destroy(terrain->mesh);
    }
    free(terrain);
}

void terrain_draw(const Terrain *terrain, const Shader *shader) {
    if (!terrain || !terrain->mesh || !shader) {
        return;
    }

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (float *) terrain->position);

    shader_set_mat4(shader, "u_model", model);
    mesh_draw(terrain->mesh);
}
