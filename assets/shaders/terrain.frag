#version 460 core

in vec3 v_world_pos;
in vec3 v_normal;

out vec4 frag_color;

void main() {
    float h = v_world_pos.y;
    vec3 color;

    if (h < -2.0) {
        color = vec3(0.12, 0.35, 0.65); // Wasser
    } else if (h < 0.0) {
        color = vec3(0.82, 0.76, 0.55); // Sand
    } else if (h < 8.0) {
        color = vec3(0.22, 0.55, 0.24); // Gras
    } else if (h < 14.0) {
        color = vec3(0.45, 0.43, 0.40); // Fels
    } else {
        color = vec3(0.95, 0.96, 0.98); // Schnee
    }

    vec3 sun_dir = normalize(vec3(0.4, 0.8, 0.3));
    vec3 norm = normalize(v_normal);
    float diff = max(dot(norm, sun_dir), 0.0);
    float ambient = 0.25;

    frag_color = vec4(color * (ambient + diff * 0.75), 1.0);
}