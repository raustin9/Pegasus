#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

layout (binding = 0) uniform UniformBufferObject {
    mat4 modelViewProjection;
} ubo;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inColor, 1.0);
    gl_Position = ubo.modelViewProjection * vec4(inPos, 1.0);
    gl_Position.y *= -1;
}
