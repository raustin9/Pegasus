#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 out_color;
layout (location = 0) in vec3 in_position;

void main() {
    // out_color = vec4(0.8, 0.3, 0.7, 1.0);
    out_color = vec4(in_position, 1.0);
}