#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec4 tangent;

layout(location = 0) out vec4 fragWorldPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexcoord;
layout(location = 3) out vec4 fragTangent;

layout(set = 0, binding = 0) uniform PerFrameUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} perFrame;

layout(push_constant) uniform PushConstants {
    mat4 modelMat;
} pc;

void main() {
    vec4 worldPos = pc.modelMat * vec4(position, 1.0f);
    gl_Position = perFrame.viewProj * worldPos;

    mat3 modelMat3 = mat3(pc.modelMat);

    fragNormal = mat3(pc.modelMat) * normal;
    fragWorldPos = worldPos;
    fragTexcoord = texcoord;
    fragTangent  = vec4(modelMat3 * tangent.xyz, tangent.w);
}