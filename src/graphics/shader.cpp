#include "../../include/graphics/shader.h"

#include <cstdio>
#include <cstdlib>

static char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("Failed to open shader file: %s\n", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    const long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (length < 0) {
        fclose(file);
        return nullptr;
    }

    const auto buffer = static_cast<char *>(malloc(static_cast<size_t>(length) + 1));
    if (!buffer) {
        fclose(file);
        return nullptr;
    }

    const size_t read_bytes = fread(buffer, 1, static_cast<size_t>(length), file);
    buffer[read_bytes] = '\0';
    fclose(file);
    return buffer;
}

static GLuint compile_shader(const GLenum type, const char *source, const char *path) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
    glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
        printf("Shader compilation error (%s):\n%s\n", path, info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

Shader *shader_new(const char *vertex_path, const char *fragment_path) {
    char *vertex_src = read_file(vertex_path);
    char *fragment_src = read_file(fragment_path);

    if (!vertex_src || !fragment_src) {
        free(vertex_src);
        free(fragment_src);
        return nullptr;
    }

    const GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_src, vertex_path);
    const GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_src, fragment_path);

    free(vertex_src);
    free(fragment_src);

    if (!vertex || !fragment) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return nullptr;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
    glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        printf("Shader linking error:\n%s\n", info_log);
        glDeleteProgram(program);
        return nullptr;
    }

    auto *shader = static_cast<Shader *>(malloc(sizeof(Shader)));
    if (!shader) {
        glDeleteProgram(program);
        return nullptr;
    }

    shader->id = program;
    return shader;
}

void shader_use(const Shader *shader) {
    if (shader) {
        glUseProgram(shader->id);
    }
}

void shader_destroy(Shader *shader) {
    if (!shader) {
        return;
    }
    glDeleteProgram(shader->id);
    free(shader);
}

void shader_set_int(const Shader *shader, const char *name, const int value) {
    if (!shader) return;
    glProgramUniform1i(shader->id, glGetUniformLocation(shader->id, name), value);
}

void shader_set_float(const Shader *shader, const char *name, const float value) {
    if (!shader) return;
    glProgramUniform1f(shader->id, glGetUniformLocation(shader->id, name), value);
}

void shader_set_vec3(const Shader *shader, const char *name, const vec3 value) {
    if (!shader) return;
    glProgramUniform3fv(shader->id, glGetUniformLocation(shader->id, name), 1, value);
}

void shader_set_mat4(const Shader *shader, const char *name, const mat4 value) {
    if (!shader) return;
    glProgramUniformMatrix4fv(shader->id, glGetUniformLocation(shader->id, name), 1, GL_FALSE, reinterpret_cast<const float *>(value));
}
