#include "core/window.h"
#include "graphics/mesh.h"
#include "world/noise.h"
#include <cstdio>
#include <new>

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

    const auto *input = static_cast<const Input *>(glfwGetWindowUserPointer(handle));
    if (Window *window = input ? input->owner : nullptr) {
        window->width = width;
        window->height = height;
    }
}

static void enable_dark_titlebar(GLFWwindow *handle) {
#ifdef _WIN32
    if (const HWND hwnd = glfwGetWin32Window(handle)) {
        constexpr BOOL value = TRUE;
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
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!handle) {
        glfwTerminate();
        return nullptr;
    }

    enable_dark_titlebar(handle);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (const GLFWvidmode *mode = glfwGetVideoMode(monitor)) {
        glfwSetWindowPos(handle, (mode->width - width) / 2, (mode->height - height) / 2);
    }

    glfwShowWindow(handle);
    glfwMakeContextCurrent(handle);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::printf("Error while initialising GLAD\n");
        glfwDestroyWindow(handle);
        glfwTerminate();
        return nullptr;
    }

    auto *window = new(std::nothrow) Window{};
    if (!window) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        return nullptr;
    }

    int fb_width = 0;
    int fb_height = 0;
    glfwGetFramebufferSize(handle, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    window->handle = handle;
    std::snprintf(window->title, sizeof(window->title), "%s", title);
    window->width = fb_width;
    window->height = fb_height;
    glEnable(GL_DEPTH_TEST);

    vec3 camera_position = {0.0f, 100.0f, 220.0f};
    window->camera = std::make_unique<Camera>(camera_position);
    if (window->camera) {
        window->camera->ProcessMouse(0.0f, -250.0f);
    }

    PerlinNoise *noise = perlin_new(1337);
    constexpr TerrainConfig config = {
        .width = 256,
        .depth = 256,
        .scale = 0.0075f,
        .max_height = 28.0f,
        .octaves = 4,
        .persistence = 0.4f,
        .lacunarity = 2.0f
    };
    vec3 terrain_pos = {-128.0f, 0.0f, -128.0f};

    window->terrain = terrain_new(noise, config, terrain_pos);
    window->terrain_shader = shader_new(ASSET_DIR "/shaders/terrain.vert", ASSET_DIR "/shaders/terrain.frag");
    window->grass_shader = shader_new(ASSET_DIR "/shaders/grass.vert", ASSET_DIR "/shaders/grass.frag");
    window->grass = grass_new(noise, config, terrain_pos, 250000, 42);
    perlin_destroy(noise);

    if (!window->camera || !window->terrain || !window->terrain_shader || !window->grass_shader || !window->grass) {
        window_destroy(window);
        return nullptr;
    }

    window->last_frame = static_cast<float>(glfwGetTime());
    glfwSetWindowUserPointer(handle, window);
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);

    window->input = input_new(handle, window->camera.get());
    if (!window->input) {
        window_destroy(window);
        return nullptr;
    }
    window->input->owner = window;

    return window;
}

void window_loop(Window *window) {
    float fps_elapsed = 0.0f;
    unsigned int fps_frames = 0;

    while (!glfwWindowShouldClose(window->handle)) {
        const auto current_frame = static_cast<float>(glfwGetTime());
        const float delta_time = current_frame - window->last_frame;
        window->last_frame = current_frame;
        fps_elapsed += delta_time;
        fps_frames++;

        if (fps_elapsed >= 1.0f) {
            char title[320];
            const float fps = static_cast<float>(fps_frames) / fps_elapsed;
            std::snprintf(title, sizeof(title), "%s | %.0f FPS", window->title, fps);
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
    if (!window || !window->terrain || !window->terrain_shader || !window->camera || window->height <= 0) {
        return;
    }

    mat4 view;
    mat4 projection;
    window->camera->GetViewMatrix(view);
    window->camera->GetProjectionMatrix(
        static_cast<float>(window->width) / static_cast<float>(window->height),
        projection
    );

    shader_use(window->terrain_shader);
    shader_set_mat4(window->terrain_shader, "u_view", view);
    shader_set_mat4(window->terrain_shader, "u_projection", projection);
    glBindVertexArray(window->terrain->mesh->vao);
    terrain_draw(window->terrain, window->terrain_shader);

    mat4 view_projection;
    glm_mat4_mul(projection, view, view_projection);
    shader_use(window->grass_shader);
    shader_set_mat4(window->grass_shader, "u_view_projection", view_projection);
    grass_draw(window->grass, window->grass_shader, static_cast<float>(glfwGetTime()));

    glBindVertexArray(0);
}

void window_toggle_fullscreen(Window *window) {
    if (!window || !window->handle) {
        return;
    }

    if (!window->fullscreen) {
        glfwGetWindowPos(window->handle, &window->windowed_x, &window->windowed_y);
        glfwGetWindowSize(window->handle, &window->windowed_width, &window->windowed_height);

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
        if (!monitor || !mode) {
            return;
        }

        glfwSetWindowMonitor(window->handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        window->fullscreen = 1;
    } else {
        glfwSetWindowMonitor(window->handle, nullptr, window->windowed_x, window->windowed_y,
                             window->windowed_width, window->windowed_height, GLFW_DONT_CARE);
        window->fullscreen = 0;
    }
}

void window_destroy(const Window *window) {
    if (!window) {
        return;
    }

    terrain_destroy(window->terrain);
    shader_destroy(window->terrain_shader);
    grass_destroy(window->grass);
    shader_destroy(window->grass_shader);
    input_destroy(window->input);

    if (window->handle) {
        glfwDestroyWindow(window->handle);
    }

    delete window;
    glfwTerminate();
}

void window_run(Window *window) {
    if (!window) {
        return;
    }
    window_loop(window);
    window_destroy(window);
}
