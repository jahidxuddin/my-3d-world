#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 uObjectColor;
uniform vec3 uLightDir;
uniform vec3 uLightColor;

void main() {
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * uLightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 result = (ambient + diffuse) * uObjectColor;
    FragColor = vec4(result, 1.0);
}