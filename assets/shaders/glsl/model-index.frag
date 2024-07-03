#version 450 core

out vec4 FragColor;

uniform int modelIdx;

void main() {
    int r = (modelIdx & 0x000000FF) >> 0;
    int g = (modelIdx & 0x0000FF00) >> 8;
    int b = (modelIdx & 0x00FF0000) >> 16;
    FragColor = vec4(float(r) / 255.0, float(g) / 255.0, float(b) / 255.0, 1.0);
}
