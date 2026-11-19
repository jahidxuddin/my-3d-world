#include "../../include/graphics/camera.h"
#include <stdlib.h>

Camera *camera_new(vec3 position) {
    Camera *camera = malloc(sizeof(Camera));
    if (!camera) {
        return NULL;
    }

    glm_vec3_copy(position, camera->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->up);
    glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, camera->right);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->world_up);

    camera->yaw = -90.0f;
    camera->pitch = 0.0f;

    camera->movement_speed = 5.0f;
    camera->mouse_sensitivity = 0.1f;
    camera->fov = 45.0f;
    camera->near_plane = 0.1f;
    camera->far_plane = 1000.0f;

    return camera;
}

void camera_destroy(Camera *camera) {
    if (!camera) {
        return;
    }
    free(camera);
}

void camera_process_keyboard(Camera *camera, const CameraMovement direction, const float delta_time) {
    if (!camera) return;

    const float velocity = camera->movement_speed * delta_time;

    switch (direction) {
        case CAMERA_FORWARD:
            glm_vec3_muladds(camera->front, velocity, camera->position);
            break;
        case CAMERA_BACKWARD:
            glm_vec3_muladds(camera->front, -velocity, camera->position);
            break;
        case CAMERA_LEFT:
            glm_vec3_muladds(camera->right, -velocity, camera->position);
            break;
        case CAMERA_RIGHT:
            glm_vec3_muladds(camera->right, velocity, camera->position);
            break;
        case CAMERA_UP:
            glm_vec3_muladds(camera->world_up, velocity, camera->position);
            break;
        case CAMERA_DOWN:
            glm_vec3_muladds(camera->world_up, -velocity, camera->position);
            break;
        default:
            break;
    }
}

static void camera_update_vectors(Camera *camera) {
    vec3 front;
    front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    front[1] = sinf(glm_rad(camera->pitch));
    front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    glm_vec3_normalize_to(front, camera->front);

    glm_vec3_crossn(camera->front, camera->world_up, camera->right);
    glm_vec3_crossn(camera->right, camera->front, camera->up);
}

void camera_process_mouse(Camera *camera, const float x_offset, const float y_offset) {
    if (!camera) return;

    camera->yaw += x_offset * camera->mouse_sensitivity;
    camera->pitch += y_offset * camera->mouse_sensitivity;

    if (camera->pitch > 89.0f) {
        camera->pitch = 89.0f;
    }
    if (camera->pitch < -89.0f) {
        camera->pitch = -89.0f;
    }

    camera_update_vectors(camera);
}

void camera_process_scroll(Camera *camera, const float y_offset) {
    if (!camera) return;

    glm_vec3_muladds(camera->front, y_offset * camera->movement_speed, camera->position);
}

void camera_get_view_matrix(const Camera *camera, mat4 dest) {
    if (!camera) return;

    vec3 target;
    glm_vec3_add((float *) camera->position, (float *) camera->front, target);
    glm_lookat((float *) camera->position, target, (float *) camera->up, dest);
}

void camera_get_projection_matrix(const Camera *camera, const float aspect_ratio, mat4 dest) {
    if (!camera) return;

    glm_perspective(glm_rad(camera->fov), aspect_ratio, camera->near_plane, camera->far_plane, dest);
}
