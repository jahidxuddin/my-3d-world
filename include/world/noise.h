#ifndef MY_3D_WORLD_NOISE_H
#define MY_3D_WORLD_NOISE_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    uint8_t perm[512];
} PerlinNoise;

PerlinNoise *perlin_new(uint32_t seed);

void perlin_destroy(PerlinNoise *noise);

float perlin_noise2d(const PerlinNoise *noise, float x, float y);

float perlin_fbm2d(const PerlinNoise *noise, float x, float y, int octaves, float persistence, float lacunarity);

#endif // MY_3D_WORLD_NOISE_H
