#include "core/window.h"
#include <new>

static void mouse_callback(GLFWwindow *window, const double xpos, const double ypos) {
    auto *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
    if (!input || !input->camera || !input->cursor_captured) {
        return;
    }

    const auto x = static_cast<float>(xpos);
    const auto y = static_cast<float>(ypos);

    if (input->first_mouse) {
        input->last_x = x;
        input->last_y = y;
        input->first_mouse = false;
    }

    const float x_offset = x - input->last_x;
    const float y_offset = input->last_y - y;

    input->last_x = x;
    input->last_y = y;

    input->camera->ProcessMouse(x_offset, y_offset);
}

static void scroll_callback(GLFWwindow *window, const double xoffset, const double yoffset) {
    (void) xoffset;
    const auto *input = static_cast<const Input *>(glfwGetWindowUserPointer(window));
    if (!input || !input->camera) {
        return;
    }

    input->camera->ProcessScroll(static_cast<float>(yoffset));
}

static void key_callback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods) {
    (void) scancode;
    (void) mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        const auto *input = static_cast<const Input *>(glfwGetWindowUserPointer(window));
        if (input && input->owner) {
            window_toggle_fullscreen(input->owner);
        }
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        auto *input = static_cast<Input *>(glfwGetWindowUserPointer(window));
        if (!input) {
            return;
        }

        input->cursor_captured = !input->cursor_captured;
        input->first_mouse = true;
        glfwSetInputMode(window, GLFW_CURSOR,
                         input->cursor_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

Input *input_new(GLFWwindow *window, Camera *camera) {
    if (!window || !camera) {
        return nullptr;
    }

    auto *input = new(std::nothrow) Input{};
    if (!input) {
        return nullptr;
    }

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
    delete input;
}

void input_process(const Input *input, const float delta_time) {
    if (!input || !input->window || !input->camera) {
        return;
    }

    GLFWwindow *win = input->window;

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        input->camera->ProcessKeyboard(CameraMovement::FORWARD, delta_time);
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        input->camera->ProcessKeyboard(CameraMovement::BACKWARD, delta_time);
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        input->camera->ProcessKeyboard(CameraMovement::LEFT, delta_time);
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        input->camera->ProcessKeyboard(CameraMovement::RIGHT, delta_time);
    }
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) {
        input->camera->ProcessKeyboard(CameraMovement::UP, delta_time);
    }
    if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        input->camera->ProcessKeyboard(CameraMovement::DOWN, delta_time);
    }
}
