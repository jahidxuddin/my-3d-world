#ifndef MY_3D_WORLD_WINDOW_H
#define MY_3D_WORLD_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "core/input.h"
#include "world/terrain.h"

struct Window {
    GLFWwindow *handle;
    char title[256];
    int width, height;
    Camera *camera;
    Terrain *terrain;
    Shader *shader;
    Input *input;
    float last_frame;
};
typedef Window Window;

Window *window_new(const char *title, int width, int height);

void window_loop(Window *window);

void window_render(const Window *window);

void window_run(Window *window);

void window_destroy(Window *window);

#endif //MY_3D_WORLD_WINDOW_H
