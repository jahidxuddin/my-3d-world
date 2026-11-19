#include "../../include/world/grass.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static float rand_float(uint32_t *state) {
    *state ^= *state << 13;
    *state ^= *state >> 17;
    *state ^= *state << 5;
    return (float) (*state) / (float) UINT32_MAX;
}

GrassField *grass_new(const PerlinNoise *noise, const TerrainConfig terrain_config, const vec3 terrain_pos,
                      const uint32_t count,
                      const uint32_t seed) {
    if (!noise || count == 0) {
        return NULL;
    }

    GrassField *grass_field = calloc(1, sizeof(GrassField));
    if (!grass_field) {
        return NULL;
    }

    const float blade_vertices[] = {
        // --- Halm 1 (Mitte) ---
        -0.06f, 0.00f, 0.00f,
        0.06f, 0.00f, 0.00f,
        -0.045f, 0.35f, 0.03f,
        0.045f, 0.35f, 0.03f,
        -0.025f, 0.70f, 0.07f,
        0.025f, 0.70f, 0.07f,
        0.00f, 1.00f, 0.12f,

        // --- Halm 2 (Links versetzt & gedreht) ---
        -0.18f, 0.00f, -0.08f,
        -0.06f, 0.00f, -0.02f,
        -0.17f, 0.32f, -0.06f,
        -0.08f, 0.32f, -0.01f,
        -0.16f, 0.65f, -0.03f,
        -0.11f, 0.65f, 0.00f,
        -0.14f, 0.90f, 0.04f,

        // --- Halm 3 (Rechts versetzt & gedreht) ---
        0.06f, 0.00f, 0.03f,
        0.18f, 0.00f, 0.09f,
        0.08f, 0.34f, 0.05f,
        0.17f, 0.34f, 0.10f,
        0.11f, 0.68f, 0.08f,
        0.16f, 0.68f, 0.12f,
        0.14f, 0.95f, 0.16f
    };

    const uint32_t blade_indices[] = {
        // Halm 1
        0, 1, 2, 1, 3, 2, 2, 3, 4, 3, 5, 4, 4, 5, 6,
        // Halm 2
        7, 8, 9, 8, 10, 9, 9, 10, 11, 10, 12, 11, 11, 12, 13,
        // Halm 3
        14, 15, 16, 15, 17, 16, 16, 17, 18, 17, 19, 18, 18, 19, 20
    };

    GrassInstance *instances = malloc(count * sizeof(GrassInstance));
    if (!instances) {
        free(grass_field);
        return NULL;
    }

    uint32_t rng = (seed == 0) ? 1 : seed;
    const float max_x = (float) (terrain_config.width - 1);
    const float max_z = (float) (terrain_config.depth - 1);

    for (uint32_t i = 0; i < count; i++) {
        const float world_x = terrain_pos[0] + rand_float(&rng) * max_x;
        const float world_z = terrain_pos[2] + rand_float(&rng) * max_z;
        const float world_y = terrain_get_height(noise, terrain_config, world_x, world_z);

        instances[i].position[0] = world_x;
        instances[i].position[1] = world_y;
        instances[i].position[2] = world_z;
        const float rotation = rand_float(&rng) * 6.2831853f;
        instances[i].rotation[0] = cosf(rotation);
        instances[i].rotation[1] = sinf(rotation);
        instances[i].scale = 0.9f + rand_float(&rng) * 0.9f;
        instances[i].color_var = rand_float(&rng);
    }

    glCreateVertexArrays(1, &grass_field->vao);
    glCreateBuffers(1, &grass_field->vbo_blade);
    glCreateBuffers(1, &grass_field->vbo_instances);
    glCreateBuffers(1, &grass_field->ebo);

    glNamedBufferData(grass_field->vbo_blade, sizeof(blade_vertices), blade_vertices, GL_STATIC_DRAW);
    glNamedBufferData(grass_field->ebo, sizeof(blade_indices), blade_indices, GL_STATIC_DRAW);
    glNamedBufferData(grass_field->vbo_instances, count * sizeof(GrassInstance), instances, GL_STATIC_DRAW);

    free(instances);

    glVertexArrayVertexBuffer(grass_field->vao, 0, grass_field->vbo_blade, 0, 3 * sizeof(float));
    glVertexArrayVertexBuffer(grass_field->vao, 1, grass_field->vbo_instances, 0, sizeof(GrassInstance));
    glVertexArrayElementBuffer(grass_field->vao, grass_field->ebo);

    // Location 0: Grashalm-Vertex (pro Vertex)
    glEnableVertexArrayAttrib(grass_field->vao, 0);
    glVertexArrayAttribFormat(grass_field->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(grass_field->vao, 0, 0);

    // Location 1..3: Instanz-Daten (pro Grashalm)
    glEnableVertexArrayAttrib(grass_field->vao, 1);
    glVertexArrayAttribFormat(grass_field->vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(GrassInstance, position));
    glVertexArrayAttribBinding(grass_field->vao, 1, 1);

    glEnableVertexArrayAttrib(grass_field->vao, 2);
    glVertexArrayAttribFormat(grass_field->vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(GrassInstance, rotation));
    glVertexArrayAttribBinding(grass_field->vao, 2, 1);

    glEnableVertexArrayAttrib(grass_field->vao, 3);
    glVertexArrayAttribFormat(grass_field->vao, 3, 1, GL_FLOAT, GL_FALSE, offsetof(GrassInstance, scale));
    glVertexArrayAttribBinding(grass_field->vao, 3, 1);

    glEnableVertexArrayAttrib(grass_field->vao, 4);
    glVertexArrayAttribFormat(grass_field->vao, 4, 1, GL_FLOAT, GL_FALSE, offsetof(GrassInstance, color_var));
    glVertexArrayAttribBinding(grass_field->vao, 4, 1);

    glVertexArrayBindingDivisor(grass_field->vao, 1, 1);

    grass_field->index_count = (uint32_t) (sizeof(blade_indices) / sizeof(blade_indices[0]));
    grass_field->instance_count = count;

    return grass_field;
}

void grass_destroy(GrassField *grass) {
    if (!grass) {
        return;
    }
    glDeleteVertexArrays(1, &grass->vao);
    glDeleteBuffers(1, &grass->vbo_blade);
    glDeleteBuffers(1, &grass->vbo_instances);
    glDeleteBuffers(1, &grass->ebo);
    free(grass);
}

void grass_draw(const GrassField *grass, const Shader *shader, const float time) {
    if (!grass || !shader || grass->instance_count == 0) {
        return;
    }

    glDisable(GL_CULL_FACE);

    shader_set_float(shader, "u_time", time);

    glBindVertexArray(grass->vao);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei) grass->index_count, GL_UNSIGNED_INT, 0,
                            (GLsizei) grass->instance_count);
    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
}
