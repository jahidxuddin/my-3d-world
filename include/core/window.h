#ifndef MY_3D_WORLD_WINDOW_H
#define MY_3D_WORLD_WINDOW_H

#include <glad/glad.h>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include "graphics/camera.h"
#include "graphics/shader.h"
#include "core/input.h"
#include "world/grass.h"
#include "world/terrain.h"

struct Window {
    GLFWwindow *handle;
    char title[256];
    int width, height;
    int windowed_x, windowed_y;
    int windowed_width, windowed_height;
    int fullscreen;
    Camera *camera;
    Terrain *terrain;
    Shader *terrain_shader;
    GrassField *grass;
    Shader *grass_shader;
    Input *input;
    float last_frame;
};
typedef Window Window;

Window *window_new(const char *title, int width, int height);

void window_loop(Window *window);

void window_render(const Window *window);

void window_run(Window *window);

void window_destroy(Window *window);

void window_toggle_fullscreen(Window *window);

#endif //MY_3D_WORLD_WINDOW_H
