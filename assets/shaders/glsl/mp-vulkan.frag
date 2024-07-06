#version 450

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec3 Normal;

layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 1) uniform sampler2D texture_diffuse;

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    bool hasTexture;
} ubo;

void main() {
    if (ubo.hasTexture) {
        FragColor = texture(texture_diffuse, TexCoords);
    } else {    // No Texture, the same as solid
        vec3 lightDir = normalize(vec3(ubo.view * vec4(-0.2, -1.0, -0.3, 0.0)));
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 ambient= vec3(0.3, 0.3, 0.3);
        vec3 diffuse = diff * vec3(0.8, 0.8, 0.8);
        vec3 result = vec3(0.6, 0.6, 0.6) * (ambient + diffuse);
        FragColor = vec4(result, 1.0);
    }
}
