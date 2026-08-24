#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec3 fragNormal;

layout(push_constant) uniform PushConstants {
    mat4 modelMat;
} pc;

void main() {
    // gl_Position = vec4(position * 0.5, 1.0);
    gl_Position = pc.modelMat * vec4(position * 0.5, 1.0);
    fragNormal = normal;
}