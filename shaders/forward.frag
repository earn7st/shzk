#version 460

layout(location = 0) in vec3 fragNormal;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragNormal * 0.5 + 0.5, 1.0);
    // outColor = vec4(1.0, 0.0, 1.0, 1.0);
}