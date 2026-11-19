#ifndef MY_3D_WORLD_TERRAIN_H
#define MY_3D_WORLD_TERRAIN_H

#include <stdint.h>
#include "../graphics/mesh.h"
#include "../graphics/shader.h"
#include "world/noise.h"

typedef struct {
    int width;
    int depth;
    float scale;
    float max_height;

    int octaves;
    float persistence;
    float lacunarity;
} TerrainConfig;

typedef struct {
    Mesh *mesh;
    TerrainConfig config;
    vec3 position;
} Terrain;

float terrain_get_height(const PerlinNoise *noise, TerrainConfig config, float world_x, float world_z);

Terrain *terrain_new(const PerlinNoise *noise, TerrainConfig config, vec3 position);

void terrain_destroy(Terrain *terrain);

void terrain_draw(const Terrain *terrain, const Shader *shader);


#endif // MY_3D_WORLD_TERRAIN_H
