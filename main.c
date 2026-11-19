#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void enable_dark_titlebar(GLFWwindow* window) {
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd) {
        BOOL value = TRUE;
        if (FAILED(DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value)))) {
            DwmSetWindowAttribute(hwnd, 19, &value, sizeof(value));
        }
    }
#else
    (void)window;
#endif
}

int main(void) {
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    const int width = 1080;
    const int height = 600;

    GLFWwindow* window = glfwCreateWindow(width, height, "My 3D World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    enable_dark_titlebar(window);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode) {
        glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
    }

    glfwShowWindow(window);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Error while initialising GLAD\n");
        return -1;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}