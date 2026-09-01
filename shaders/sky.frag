#version 460

layout(location = 0) in vec3 vWorldDir;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform PerFrameUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} perFrame;

layout(set = 1, binding = 10) uniform samplerCube texCube;     // sRGB

void main()
{
    vec3 color = texture(texCube, normalize(vWorldDir)).rgb;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}
