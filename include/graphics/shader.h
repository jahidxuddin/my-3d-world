#ifndef MY_3D_WORLD_SHADER_H
#define MY_3D_WORLD_SHADER_H

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
    GLuint id;
} Shader;

Shader *shader_new(const char *vertex_path, const char *fragment_path);

void shader_use(const Shader *shader);

void shader_destroy(Shader *shader);

void shader_set_int(const Shader *shader, const char *name, int value);

void shader_set_float(const Shader *shader, const char *name, float value);

void shader_set_vec3(const Shader *shader, const char *name, const vec3 value);

void shader_set_mat4(const Shader *shader, const char *name, const mat4 value);

#endif // MY_3D_WORLD_SHADER_H
