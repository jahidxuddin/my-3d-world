#include "../../include/world/noise.h"

PerlinNoise *perlin_new(const uint32_t seed) {
    PerlinNoise *perlin_noise = malloc(sizeof(PerlinNoise));
    if (!perlin_noise) {
        return NULL;
    }

    for (uint32_t i = 0; i < 256; i++) {
        perlin_noise->perm[i] = (uint8_t) i;
    }

    uint32_t state = (seed == 0) ? 1 : seed;

    for (uint32_t i = 255; i > 0; i--) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;

        const uint32_t j = state % (i + 1);
        const uint8_t tmp = perlin_noise->perm[i];
        perlin_noise->perm[i] = perlin_noise->perm[j];
        perlin_noise->perm[j] = tmp;
    }

    for (uint32_t i = 0; i < 256; i++) {
        perlin_noise->perm[i + 256] = perlin_noise->perm[i];
    }

    return perlin_noise;
}

void perlin_destroy(PerlinNoise *noise) {
    if (!noise) {
        return;
    }
    free(noise);
}

static float fade(const float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static float lerp(const float a, const float b, const float t) {
    return a + t * (b - a);
}

static float grad(const uint8_t hash, const float x, const float y) {
    switch (hash & 7) {
        case 0: return x + y;
        case 1: return -x + y;
        case 2: return x - y;
        case 3: return -x - y;
        case 4: return x;
        case 5: return -x;
        case 6: return y;
        case 7: return -y;
        default: return 0.0f;
    }
}

float perlin_noise2d(const PerlinNoise *noise, float x, float y) {
    if (!noise) {
        return 0.0f;
    }

    const int xi = (int) floorf(x);
    const int yi = (int) floorf(y);

    const uint8_t X = (uint8_t) (xi & 255);
    const uint8_t Y = (uint8_t) (yi & 255);

    const float xf = x - (float) xi;
    const float yf = y - (float) yi;

    const float u = fade(xf);
    const float v = fade(yf);

    const uint8_t *p = noise->perm;
    const uint8_t aa = p[p[X] + Y];
    const uint8_t ab = p[p[X] + Y + 1];
    const uint8_t ba = p[p[X + 1] + Y];
    const uint8_t bb = p[p[X + 1] + Y + 1];

    const float x1 = lerp(grad(aa, xf, yf),
                          grad(ba, xf - 1.0f, yf),
                          u);
    const float x2 = lerp(grad(ab, xf, yf - 1.0f),
                          grad(bb, xf - 1.0f, yf - 1.0f),
                          u);

    return lerp(x1, x2, v);
}

float perlin_fbm2d(const PerlinNoise *noise, const float x, const float y,
                   const int octaves, const float persistence, const float lacunarity) {
    if (!noise || octaves <= 0) {
        return 0.0f;
    }

    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float max_value = 0.0f;

    for (int i = 0; i < octaves; i++) {
        total += perlin_noise2d(noise, x * frequency, y * frequency) * amplitude;

        max_value += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return total / max_value;
}
