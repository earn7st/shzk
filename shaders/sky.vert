#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 tangent;

layout(location = 0) out vec3 vWorldDir;

layout(set = 0, binding = 0) uniform PerFrameUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} perFrame;

void main() {

    vWorldDir = position;

    mat3 viewRotation  = mat3(perFrame.view);
    vec4 clip = perFrame.proj * vec4(viewRotation * position, 1.0);
    clip.z = 0.0;
    gl_Position = clip;
}