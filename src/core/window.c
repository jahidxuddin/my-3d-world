#include "core/window.h"
#include <stdio.h>
#include <stdlib.h>

static const Vertex cube_vertices[] = {
    {{-0.5f,-0.5f, 0.5f},{0,0,1},{0,0}}, {{0.5f,-0.5f, 0.5f},{0,0,1},{1,0}}, {{0.5f,0.5f,0.5f},{0,0,1},{1,1}}, {{-0.5f,0.5f,0.5f},{0,0,1},{0,1}},
    {{0.5f,-0.5f,-0.5f},{0,0,-1},{0,0}}, {{-0.5f,-0.5f,-0.5f},{0,0,-1},{1,0}}, {{-0.5f,0.5f,-0.5f},{0,0,-1},{1,1}}, {{0.5f,0.5f,-0.5f},{0,0,-1},{0,1}},
    {{-0.5f,-0.5f,-0.5f},{-1,0,0},{0,0}}, {{-0.5f,-0.5f,0.5f},{-1,0,0},{1,0}}, {{-0.5f,0.5f,0.5f},{-1,0,0},{1,1}}, {{-0.5f,0.5f,-0.5f},{-1,0,0},{0,1}},
    {{0.5f,-0.5f,0.5f},{1,0,0},{0,0}}, {{0.5f,-0.5f,-0.5f},{1,0,0},{1,0}}, {{0.5f,0.5f,-0.5f},{1,0,0},{1,1}}, {{0.5f,0.5f,0.5f},{1,0,0},{0,1}},
    {{-0.5f,0.5f,0.5f},{0,1,0},{0,0}}, {{0.5f,0.5f,0.5f},{0,1,0},{1,0}}, {{0.5f,0.5f,-0.5f},{0,1,0},{1,1}}, {{-0.5f,0.5f,-0.5f},{0,1,0},{0,1}},
    {{-0.5f,-0.5f,-0.5f},{0,-1,0},{0,0}}, {{0.5f,-0.5f,-0.5f},{0,-1,0},{1,0}}, {{0.5f,-0.5f,0.5f},{0,-1,0},{1,1}}, {{-0.5f,-0.5f,0.5f},{0,-1,0},{0,1}}
};
static const unsigned int cube_indices[] = {
    0,1,2,2,3,0, 4,5,6,6,7,4, 8,9,10,10,11,8,
    12,13,14,14,15,12, 16,17,18,18,19,16, 20,21,22,22,23,20
};

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

static void framebuffer_size_callback(GLFWwindow *handle, const int width, const int height) {
    glViewport(0, 0, width, height);

    const Input *input = glfwGetWindowUserPointer(handle);
    Window *window = input ? input->owner : NULL;
    if (window) {
        window->width = width;
        window->height = height;
    }
}

static void enable_dark_titlebar(GLFWwindow *handle) {
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(handle);
    if (hwnd) {
        const BOOL value = TRUE;
        if (FAILED(DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value)))) {
            DwmSetWindowAttribute(hwnd, 19, &value, sizeof(value));
        }
    }
#else
    (void) handle;
#endif
}

Window *window_new(const char *title, const int width, const int height) {
    if (!glfwInit()) {
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!handle) {
        glfwTerminate();
        return NULL;
    }

    enable_dark_titlebar(handle);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (mode) {
        glfwSetWindowPos(handle, (mode->width - width) / 2, (mode->height - height) / 2);
    }

    glfwShowWindow(handle);
    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Error while initialising GLAD\n");
        glfwDestroyWindow(handle);
        glfwTerminate();
        return NULL;
    }

    Window *window = malloc(sizeof(Window));
    if (!window) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        return NULL;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(handle, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    window->handle = handle;
    snprintf(window->title, sizeof(window->title), "%s", title);
    window->width = fbWidth;
    window->height = fbHeight;
    glEnable(GL_DEPTH_TEST);
    const vec3 camera_position = {0.0f, 100.0f, 220.0f};
    window->camera = camera_new((float *) camera_position);
    if (window->camera) {
        camera_process_mouse(window->camera, 0.0f, -250.0f);
    }
    const PerlinNoise *noise = perlin_new(1337);
    const TerrainConfig config = {
        .width       = 256,
        .depth       = 256,
        .scale       = 0.0075f,
        .max_height  = 28.0f,
        .octaves     = 4,
        .persistence = 0.4f,
        .lacunarity  = 2.0f
    };
    vec3 terrain_pos = { -128.0f, 0.0f, -128.0f };
    window->terrain = terrain_new(noise, config, terrain_pos);
    window->shader = shader_new(ASSET_DIR "/shaders/terrain.vert", ASSET_DIR "/shaders/terrain.frag");
    if (!window->camera || !window->terrain || !window->shader) {
        camera_destroy(window->camera);
        terrain_destroy(window->terrain);
        shader_destroy(window->shader);
        glfwDestroyWindow(handle);
        free(window);
        glfwTerminate();
        return NULL;
    }
    window->last_frame = (float) glfwGetTime();
    glfwSetWindowUserPointer(handle, window);
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);
    window->input = input_new(handle, window->camera);
    if (!window->input) {
        camera_destroy(window->camera);
        terrain_destroy(window->terrain);
        shader_destroy(window->shader);
        glfwDestroyWindow(handle);
        free(window);
        glfwTerminate();
        return NULL;
    }
    window->input->owner = window;

    return window;
}

void window_loop(Window *window) {
    float fps_elapsed = 0.0f;
    unsigned int fps_frames = 0;

    while (!glfwWindowShouldClose(window->handle)) {
        const float current_frame = (float) glfwGetTime();
        const float delta_time = current_frame - window->last_frame;
        window->last_frame = current_frame;
        fps_elapsed += delta_time;
        fps_frames++;

        if (fps_elapsed >= 1.0f) {
            char title[320];
            const float fps = (float) fps_frames / fps_elapsed;
            snprintf(title, sizeof(title), "%s | %.0f FPS", window->title, fps);
            glfwSetWindowTitle(window->handle, title);
            fps_elapsed = 0.0f;
            fps_frames = 0;
        }

        input_process(window->input, delta_time);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window_render(window);

        glfwSwapBuffers(window->handle);
        glfwPollEvents();
    }
}

void window_render(const Window *window) {
    if (!window || !window->terrain || !window->shader || !window->camera || window->height <= 0) return;
    mat4 view, projection;
    camera_get_view_matrix(window->camera, view);
    camera_get_projection_matrix(window->camera, (float) window->width / (float) window->height, projection);
    shader_use(window->shader);
    shader_set_mat4(window->shader, "u_view", view);
    shader_set_mat4(window->shader, "u_projection", projection);
    glBindVertexArray(window->terrain->mesh->vao);
    terrain_draw(window->terrain, window->shader);
    glBindVertexArray(0);
}

void window_destroy(Window *window) {
    if (!window) {
        return;
    }
    if (window->handle) {
        camera_destroy(window->camera);
        terrain_destroy(window->terrain);
        shader_destroy(window->shader);
        input_destroy(window->input);
        glfwDestroyWindow(window->handle);
    }
    free(window);
    glfwTerminate();
}

void window_run(Window *window) {
    if (!window) {
        return;
    }
    window_loop(window);
    window_destroy(window);
}
