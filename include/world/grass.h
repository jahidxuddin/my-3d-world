#ifndef MY_3D_WORLD_GRASS_H
#define MY_3D_WORLD_GRASS_H

#include <stdint.h>
#include <glad/glad.h>
#include <cglm/cglm.h>
#include "graphics/shader.h"
#include "world/noise.h"
#include "world/terrain.h"

typedef struct {
    float position[3]; // Weltposition
    float rotation[2]; // cos/sin der Y-Rotation, vorab berechnet
    float scale; // Größenfaktor
    float color_var; // vorab berechnete Farbvariation
} GrassInstance;

typedef struct {
    GLuint vao;
    GLuint vbo_blade;
    GLuint vbo_instances;
    GLuint ebo;
    uint32_t index_count;
    uint32_t instance_count;
} GrassField;

GrassField *grass_new(const PerlinNoise *noise, TerrainConfig terrain_config,
                      const vec3 terrain_pos, uint32_t count, uint32_t seed);

void grass_destroy(GrassField *grass);

void grass_draw(const GrassField *grass, const Shader *shader, float time);

#endif // MY_3D_WORLD_GRASS_H
