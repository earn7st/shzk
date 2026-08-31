#version 460

layout(location = 0) in vec4 fragWorldPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexcoord;
layout(location = 3) in vec4 fragTangent;

layout(location = 0) out vec4 outColor;

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

layout(set = 1, binding = 1) uniform sampler2D texBaseColor;    // sRGB
layout(set = 1, binding = 2) uniform sampler2D texArm;          // linear
layout(set = 1, binding = 3) uniform sampler2D texNormal;       // linear
layout(set = 1, binding = 4) uniform sampler2D texOcclusion;
layout(set = 1, binding = 5) uniform sampler2D texEmissive;

layout(push_constant) uniform PushConstants {
    mat4 modelMat;
} pc;

const float kPi = 3.14159265359;

struct DirectionalLight {
    vec3 L    ;
    vec3 radiance;
};

DirectionalLight GetSun()
{
    DirectionalLight sun;
    sun.L        = normalize(vec3(0.5, 1.0, 5.0));
    sun.radiance = vec3(4.0);
    return sun;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a   = roughness * roughness;
    float a2  = a * a;
    float NdH = max(dot(N, H), 0.0);
    float d   = NdH * NdH * (a2 - 1.0) + 1.0;
    return a2 / (kPi * d * d);
}

float GeometrySchlickGGX(float NdV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdV / (NdV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdV = max(dot(N, V), 0.0);
    float NdL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdV, roughness) * GeometrySchlickGGX(NdL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 GetNormalFromMap()
{
    vec3 N = normalize(fragNormal);
    vec3 T = normalize(fragTangent.xyz);
    vec3 B = fragTangent.w * normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    vec3 n = texture(texNormal, fragTexcoord).rgb * 2.0 - 1.0;
    return normalize(TBN * n);
}

void main()
{
    vec3 N = GetNormalFromMap();
    vec3 camPos = -transpose(mat3(perFrame.view)) * perFrame.view[3].xyz;
    vec3 V = normalize(camPos - fragWorldPos.xyz);

    vec4 albedoTex  = texture(texBaseColor, fragTexcoord);
    float alpha = material.baseColor.a * albedoTex.a;
    if (material.ints0.x == 1 && alpha < material.alphaCutoff)  // TODO: move ints re-definition
    {
        discard;
    }

    vec3 albedo     = material.baseColor.rgb * albedoTex.rgb;
    float ao        = texture(texOcclusion, fragTexcoord).r;
    //float roughness = material.roughness;
    //float metallic = material.metallic;
    float roughness = material.roughness * texture(texArm, fragTexcoord).g;
    float metallic  = material.metallic  * texture(texArm, fragTexcoord).b;
    roughness = max(roughness, 0.045);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    DirectionalLight sun = GetSun();
    vec3 L = sun.L;
    vec3 H = normalize(V + L);

    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3  F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3  specular = (D * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);
    vec3  kS = F;
    vec3  kD = (vec3(1.0) - kS) * (1.0 - metallic);

    float NdL = max(dot(N, L), 0.0);
    vec3  Lo  = (kD * albedo / kPi + specular) * sun.radiance * NdL;

    vec3 emissive = material.emission.rgb * texture(texEmissive, fragTexcoord).rgb;
    vec3 ambient  = vec3(0.03) * albedo * ao;
    vec3 color    = ambient + Lo + emissive;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, alpha);
}