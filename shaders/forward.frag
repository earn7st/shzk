#version 460

layout(location = 0) in vec3 fragWorldPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexcoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform PerFrameUBO
{
    mat4 viewProj;
}

layout(set = 1, binding = 0) uniform MaterialUBO 
{
    vec4  baseColor;
    vec4  emission;         // w empty
    float metallic; 
    float roughness;
    float alphaCutoff;
    uint bUseVertexColor;
    ivec4 ints0;
    ivec4 ints1;
    vec4  floats0; 
    vec4  floats1;
    vec4  colors[8];
} material;

layout(set = 1, binding = 1) uniform sampler2D texDiffuse;
layout(set = 1, binding = 2) uniform sampler2D texNormal;
layout(set = 1, binding = 3) uniform sampler2D texArm;
layout(set = 1, binding = 4) uniform sampler2D texSpecular;

layout(set = 1, binding = 5)  uniform sampler2D tex2D_0;
layout(set = 1, binding = 6)  uniform sampler2D tex2D_1;

layout(push_constant) uniform PushConstants 
{
      mat4 modelMat;
} pc;

void main() 
{
    vec4 albedo = material.baseColor * texture(texDiffuse, fragTexcoord);
    outColor = albedo;
}