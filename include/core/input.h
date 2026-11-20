#ifndef MY_3D_WORLD_INPUT_H
#define MY_3D_WORLD_INPUT_H

#include <stdbool.h>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include "graphics/camera.h"

typedef struct Window Window;

typedef struct {
    GLFWwindow *window;
    Camera *camera;
    Window *owner;

    float last_x;
    float last_y;
    bool first_mouse;
    bool cursor_captured;
} Input;

Input *input_new(GLFWwindow *window, Camera *camera);

void input_destroy(Input *input);

void input_process(const Input *input, float delta_time);

#endif // MY_3D_WORLD_INPUT_H
