#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_instance_pos;
layout (location = 2) in vec2 a_instance_rotation;
layout (location = 3) in float a_instance_scale;
layout (location = 4) in float a_color_var;

out vec3 v_color;

uniform mat4 u_view_projection;
uniform float u_time;

void main() {
    vec3 pos = a_pos * a_instance_scale;
    vec3 rotated = vec3(
        pos.x * a_instance_rotation.x - pos.z * a_instance_rotation.y,
        pos.y,
        pos.x * a_instance_rotation.y + pos.z * a_instance_rotation.x
    );

    float phase = a_color_var * 6.2831853;
    float gust = sin(u_time * 0.55 + dot(a_instance_pos.xz, vec2(0.035, 0.027)) + phase);
    float flutter = sin(u_time * 1.7 + dot(a_instance_pos.xz, vec2(0.12, 0.09)) + phase * 1.37);
    float height = clamp(a_pos.y, 0.0, 1.0);
    float bend = height * height;
    rotated.x += (gust * 0.16 + flutter * 0.07) * bend * a_instance_scale;
    rotated.z += (gust * 0.08 - flutter * 0.11) * bend * a_instance_scale;

    vec3 root_color = vec3(0.05, 0.16, 0.04);
    vec3 tip_fresh = vec3(0.28, 0.62, 0.14);
    vec3 tip_warm = vec3(0.45, 0.58, 0.15);
    vec3 tip_color = mix(tip_fresh, tip_warm, a_color_var);
    tip_color += vec3(0.06, 0.08, 0.02) * (flutter * 0.5 + 0.5) * height;
    v_color = mix(root_color, tip_color, smoothstep(0.0, 1.0, height));

    gl_Position = u_view_projection * vec4(rotated + a_instance_pos, 1.0);
}
