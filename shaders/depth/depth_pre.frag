#version 460

layout(location = 0) in vec4 fragWorldPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexcoord;

layout(set = 0, binding = 0) uniform PerFrameUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} perFrame;

layout(set = 1, binding = 0) uniform MaterialUBO {
    vec4  baseColor;
    vec4  emission;         // w empty
    float metallic;
    float roughness;
    float alphaCutoff;
    uint  bUseVertexColor;
    ivec4 ints0;
    ivec4 ints1;
    vec4  floats0;
    vec4  floats1;
    vec4  colors[8];
} material;

layout(set = 1, binding = 1) uniform sampler2D texBaseColor;

layout(push_constant) uniform PushConstants {
    mat4 modelMat;
} pc;

void main()
{
    vec4 albedoTex  = texture(texBaseColor, fragTexcoord);
    float alpha = material.baseColor.a * albedoTex.a;
    if (material.ints0.x == 1 && alpha < material.alphaCutoff)
    {
        discard;
    }
}
