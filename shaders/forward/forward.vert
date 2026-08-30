#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec3 fragWorldPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexcoord;

layout(set = 0, binding = 0) uniform PerFrameUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} perFrame;

layout(push_constant) uniform PushConstants {
    mat4 modelMat;
} pc;

void main() {
    vec4 worldPos = pc.modelMat * vec4(position, 1.0);
    gl_Position = perFrame.viewProj * worldPos;

    fragNormal = mat3(pc.modelMat) * normal;
    fragWorldPos = worldPos.xyz;
    fragTexcoord = texcoord;
}