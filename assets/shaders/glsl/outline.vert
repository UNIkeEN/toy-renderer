#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float offset;

void main() {
    vec3 normal = normalize(aPos); 
    vec4 pos = model * vec4(aPos, 1.0);
    pos.xyz += normal * offset;
    gl_Position = projection * view * pos;
}
