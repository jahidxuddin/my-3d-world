#version 460 core

in vec3 v_world_pos;
in vec3 v_normal;

out vec4 frag_color;

void main() {
    float h = v_world_pos.y;
    vec3 color = vec3(0.22, 0.55, 0.24);

    vec3 sun_dir = normalize(vec3(0.4, 0.8, 0.3));
    vec3 norm = normalize(v_normal);
    float diff = max(dot(norm, sun_dir), 0.0);
    float ambient = 0.25;

    frag_color = vec4(color * (ambient + diff * 0.75), 1.0);
}