#version 450

layout (location = 0) in vec4 inFragColor;

layout (location = 0) out vec4 outFragColor;

void main() {
    outFragColor = inFragColor;
}
