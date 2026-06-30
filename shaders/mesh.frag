 #version 450

  // ©¤©¤©¤©¤ Inputs from vertex shader ©¤©¤©¤©¤
  layout(location = 0) in vec3 fragWorldPos;
  layout(location = 1) in vec3 fragNormal;
  layout(location = 2) in vec4 fragTangent;
  layout(location = 3) in vec2 fragTexCoord;

  // ©¤©¤©¤©¤ Output ©¤©¤©¤©¤
  layout(location = 0) out vec4 outColor;

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

  // ©¤©¤©¤©¤ Set 1 = Material ©¤©¤©¤©¤
  layout(set = 1, binding = 0) uniform MaterialData {
      vec4  baseColorFactor;       // offset 0
      float metallicFactor;        // offset 16
      float roughnessFactor;       // offset 20
      float normalScale;           // offset 24
      float occlusionStrength;     // offset 28
      vec4  emissiveFactor;        // offset 32
      float emissiveStrength;      // offset 48
      float alphaCutoff;           // offset 52
      int   alphaMode;             // offset 56
      int   padding;               // offset 60 ¡ª total 64
  } material;

  layout(set = 1, binding = 1) uniform sampler2D baseColorSampler;
  layout(set = 1, binding = 2) uniform sampler2D normalSampler;
  layout(set = 1, binding = 3) uniform sampler2D metallicRoughnessSampler;
  layout(set = 1, binding = 4) uniform sampler2D occlusionSampler;
  layout(set = 1, binding = 5) uniform sampler2D emissiveSampler;

  void main()
  {

      vec4 baseColor = texture(baseColorSampler, fragTexCoord) *
  material.baseColorFactor;


      vec3 N = normalize(fragNormal);
      vec3 T = normalize(fragTangent.xyz);

      T = normalize(T - dot(T, N) * N);
      vec3 B = cross(N, T) * fragTangent.w;    // w = handedness sign from glTF

      vec3 tn = texture(normalSampler, fragTexCoord).xyz * 2.0 - 1.0;
      tn.xy *= material.normalScale;
      mat3 TBN = mat3(T, B, N);
      N = normalize(TBN * tn);

      vec3  L           = normalize(frame.sunDirection.xyz);
      float NdotL       = max(dot(N, L), 0.0);
      float sunIntensity = frame.sunDirection.w;

      vec3 diffuse = baseColor.rgb * NdotL * frame.sunColor.rgb * sunIntensity;

      vec3 ambient = baseColor.rgb * frame.ambientColor.rgb * frame.ambientColor.w;

      float ao = texture(occlusionSampler, fragTexCoord).r;
      ao = mix(1.0, ao, material.occlusionStrength);

      vec3 emissive = texture(emissiveSampler, fragTexCoord).rgb
                      * material.emissiveFactor.rgb
                      * material.emissiveStrength;

      vec3 color = (ambient + diffuse) * ao + emissive;

      outColor = vec4(color, baseColor.a);
  }