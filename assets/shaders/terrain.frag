#version 460 core

in vec3 v_world_pos;
in vec3 v_normal;

out vec4 frag_color;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float value_noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main() {
    float n = value_noise(v_world_pos.xz * 0.4) * 0.7
    + value_noise(v_world_pos.xz * 1.5) * 0.3;

    vec3 color = mix(vec3(0.06, 0.20, 0.04), vec3(0.12, 0.30, 0.07), n);

    vec3 sun_dir = normalize(vec3(0.4, 0.8, 0.3));
    vec3 norm = normalize(v_normal);
    float diff = max(dot(norm, sun_dir), 0.0);
    float ambient = 0.25;

    frag_color = vec4(color * (ambient + diff * 0.75), 1.0);
}