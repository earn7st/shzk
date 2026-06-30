#version 450

  // ©¤©¤©¤©¤ Vertex inputs (matches C++ Vertex struct / Pipeline.cpp attributes) ©¤©¤©¤©¤
  layout(location = 0) in vec3 inPosition;
  layout(location = 1) in vec3 inNormal;
  layout(location = 2) in vec4 inTangent;
  layout(location = 3) in vec2 inTexCoord;

  // ©¤©¤©¤©¤ Outputs to fragment shader ©¤©¤©¤©¤
  layout(location = 0) out vec3 fragWorldPos;
  layout(location = 1) out vec3 fragNormal;
  layout(location = 2) out vec4 fragTangent;
  layout(location = 3) out vec2 fragTexCoord;

  // ©¤©¤©¤©¤ Push constants: model matrix ©¤©¤©¤©¤
  layout(push_constant) uniform PushConstants {
      mat4 model;
  } push;

  // ©¤©¤©¤©¤ Set 0 = FrameData UBO ©¤©¤©¤©¤
  layout(set = 0, binding = 0) uniform FrameData {
      mat4 view;
      mat4 proj;
      mat4 viewProj;
      mat4 invViewProj;
      vec4 cameraPosition;
      vec4 time;
      vec4 resolution;
      vec4 ambientColor;
      vec4 sunDirection;
      vec4 sunColor;
  } frame;

  void main()
  {
      vec4 worldPos = push.model * vec4(inPosition, 1.0);
      gl_Position   = frame.viewProj * worldPos;

      fragWorldPos = worldPos.xyz;
      fragNormal   = mat3(push.model) * inNormal;
      fragTangent  = vec4(mat3(push.model) * inTangent.xyz, inTangent.w);
      fragTexCoord = inTexCoord;
  }