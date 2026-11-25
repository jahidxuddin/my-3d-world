#pragma once

#include <cglm/cglm.h>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
    vec3 position{};
    vec3 front{0.0f, 0.0f, -1.0f};
    vec3 up{0.0f, 1.0f, 0.0f};
    vec3 right{1.0f, 0.0f, 0.0f};
    vec3 world_up{0.0f, 1.0f, 0.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;

    float movement_speed = 5.0f;
    float mouse_sensitivity = 0.1f;
    float fov = 45.0f;
    float near_plane = 0.1f;
    float far_plane = 1000.0f;

    void UpdateVectors();

public:
    explicit Camera(vec3 position);

    void ProcessKeyboard(CameraMovement direction, float delta_time);

    void ProcessMouse(float x_offset, float y_offset);

    void ProcessScroll(float y_offset);

    void GetViewMatrix(mat4 dest) const;

    void GetProjectionMatrix(float aspect_ratio, mat4 dest) const;
};
