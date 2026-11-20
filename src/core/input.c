#include "core/window.h"

static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    Input *input = glfwGetWindowUserPointer(window);
    if (!input || !input->camera || !input->cursor_captured) return;

    const float x = (float) xpos;
    const float y = (float) ypos;

    if (input->first_mouse) {
        input->last_x = x;
        input->last_y = y;
        input->first_mouse = false;
    }

    const float x_offset = x - input->last_x;
    const float y_offset = input->last_y - y;

    input->last_x = x;
    input->last_y = y;

    camera_process_mouse(input->camera, x_offset, y_offset);
}

static void scroll_callback(GLFWwindow *window, const double xoffset, const double yoffset) {
    (void) xoffset;
    Input *input = glfwGetWindowUserPointer(window);
    if (!input || !input->camera) return;

    camera_process_scroll(input->camera, (float) yoffset);
}

static void key_callback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods) {
    (void) scancode;
    (void) mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        Input *input = glfwGetWindowUserPointer(window);
        if (input && input->owner) window_toggle_fullscreen(input->owner);
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        Input *input = glfwGetWindowUserPointer(window);
        if (!input) return;

        input->cursor_captured = !input->cursor_captured;
        input->first_mouse = true;
        glfwSetInputMode(window, GLFW_CURSOR,
                         input->cursor_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

Input *input_new(GLFWwindow *window, Camera *camera) {
    if (!window || !camera) return NULL;

    Input *input = malloc(sizeof(Input));
    if (!input) return NULL;

    input->window = window;
    input->camera = camera;
    input->last_x = 0.0f;
    input->last_y = 0.0f;
    input->first_mouse = true;
    input->cursor_captured = true;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window, input);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    return input;
}

void input_destroy(Input *input) {
    if (!input) return;
    free(input);
}

void input_process(const Input *input, const float delta_time) {
    if (!input || !input->window || !input->camera) return;

    GLFWwindow *win = input->window;

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        camera_process_keyboard(input->camera, CAMERA_FORWARD, delta_time);
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        camera_process_keyboard(input->camera, CAMERA_BACKWARD, delta_time);
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        camera_process_keyboard(input->camera, CAMERA_LEFT, delta_time);
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        camera_process_keyboard(input->camera, CAMERA_RIGHT, delta_time);
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera_process_keyboard(input->camera, CAMERA_UP, delta_time);
    if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera_process_keyboard(input->camera, CAMERA_DOWN, delta_time);
}
