#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;  

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float offset;

void main() {
    vec3 normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec4 pos = model * vec4(aPos + normal * offset, 1.0);
    gl_Position = projection * view * pos;
}
