#include "../../include/graphics/camera.h"
#include <cstdlib>

Camera::Camera(vec3 position) {
    glm_vec3_copy(position, this->position);
    UpdateVectors();
}

void Camera::UpdateVectors() {
    vec3 updated_front;
    updated_front[0] = cosf(glm_rad(this->yaw)) * cosf(glm_rad(this->pitch));
    updated_front[1] = sinf(glm_rad(this->pitch));
    updated_front[2] = sinf(glm_rad(this->yaw)) * cosf(glm_rad(this->pitch));
    glm_vec3_normalize_to(updated_front, this->front);

    glm_vec3_crossn(this->front, this->world_up, this->right);
    glm_vec3_crossn(this->right, this->front, this->up);
}

void Camera::ProcessKeyboard(const CameraMovement direction, const float delta_time) {
    const float velocity = this->movement_speed * delta_time;

    switch (direction) {
        case CameraMovement::FORWARD:
            glm_vec3_muladds(this->front, velocity, this->position);
            break;
        case CameraMovement::BACKWARD:
            glm_vec3_muladds(this->front, -velocity, this->position);
            break;
        case CameraMovement::LEFT:
            glm_vec3_muladds(this->right, -velocity, this->position);
            break;
        case CameraMovement::RIGHT:
            glm_vec3_muladds(this->right, velocity, this->position);
            break;
        case CameraMovement::UP:
            glm_vec3_muladds(this->world_up, velocity, this->position);
            break;
        case CameraMovement::DOWN:
            glm_vec3_muladds(this->world_up, -velocity, this->position);
            break;
        default:
            break;
    }
}

void Camera::ProcessMouse(const float x_offset, const float y_offset) {
    this->yaw += x_offset * this->mouse_sensitivity;
    this->pitch += y_offset * this->mouse_sensitivity;

    if (this->pitch > 89.0f) {
        this->pitch = 89.0f;
    }
    if (this->pitch < -89.0f) {
        this->pitch = -89.0f;
    }

    UpdateVectors();
}

void Camera::ProcessScroll(const float y_offset) {
    glm_vec3_muladds(this->front, y_offset * this->movement_speed, this->position);
}

void Camera::GetViewMatrix(mat4 dest) const {
    vec3 target;
    glm_vec3_add(const_cast<float *>(this->position), const_cast<float *>(this->front), target);
    glm_lookat(const_cast<float *>(this->position), target, const_cast<float *>(this->up), dest);
}

void Camera::GetProjectionMatrix(const float aspect_ratio, mat4 dest) const {
    glm_perspective(glm_rad(this->fov), aspect_ratio, this->near_plane, this->far_plane, dest);
}
