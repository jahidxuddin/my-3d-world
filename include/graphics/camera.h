#ifndef MY_3D_WORLD_CAMERA_H
#define MY_3D_WORLD_CAMERA_H
#include <cglm/cglm.h>

typedef enum {
    CAMERA_FORWARD,
    CAMERA_BACKWARD,
    CAMERA_LEFT,
    CAMERA_RIGHT,
    CAMERA_UP,
    CAMERA_DOWN
} CameraMovement;

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;

    float yaw;
    float pitch;

    float movement_speed;
    float mouse_sensitivity;
    float fov;
    float near_plane;
    float far_plane;
} Camera;

Camera *camera_new(vec3 position);

void camera_destroy(Camera *camera);

void camera_process_keyboard(Camera *camera, CameraMovement direction, float delta_time);

void camera_process_mouse(Camera *camera, float x_offset, float y_offset);

void camera_process_scroll(Camera *camera, float y_offset);

void camera_get_view_matrix(const Camera *camera, mat4 dest);

void camera_get_projection_matrix(const Camera *camera, float aspect_ratio, mat4 dest);

#endif //MY_3D_WORLD_CAMERA_H
