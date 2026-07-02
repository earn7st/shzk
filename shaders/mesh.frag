  #version 450

      layout(location = 0) in vec3 fragWorldPos;
      layout(location = 1) in vec3 fragNormal;
      layout(location = 2) in vec4 fragTangent;
      layout(location = 3) in vec2 fragTexCoord;
      layout(location = 0) out vec4 outColor;

      const float PI = 3.14159265359;

      layout(set = 0, binding = 0) uniform FrameData {
          mat4 view;
          mat4 proj;
          mat4 viewProj;
          mat4 invViewProj;
          vec4 cameraPosition;      // .xyz = world pos, .w = near plane
          vec4 time;                // .x = elapsed, .y = delta, .z = sin(elapsed), .w = frame counter
          vec4 resolution;          // .xy = viewport px, .zw = 1.0/viewport px
          vec4 ambientColor;        // .rgb = color, .a = intensity
          vec4 sunDirection;        // .xyz = light dir, .w = intensity
          vec4 sunColor;            // .rgb = color, .w = reserved
      } frame;


      layout(set = 1, binding = 0) uniform MaterialData {
          vec4  baseColorFactor;
          float metallicFactor;
          float roughnessFactor;
          float normalScale;
          float occlusionStrength;
          vec4  emissiveFactor;
          float emissiveStrength;
          float alphaCutoff;
          int   alphaMode;
          int   padding;
      } material;

      layout(set = 1, binding = 1) uniform sampler2D baseColorSampler;
      layout(set = 1, binding = 2) uniform sampler2D normalSampler;
      layout(set = 1, binding = 3) uniform sampler2D metallicRoughnessSampler;
      layout(set = 1, binding = 4) uniform sampler2D occlusionSampler;
      layout(set = 1, binding = 5) uniform sampler2D emissiveSampler;


      // Trowbridge-Reitz GGX normal distribution
      float DistributionGGX(vec3 N, vec3 H, float roughness)
      {
          float a      = roughness * roughness;
          float a2     = a * a;
          float NdotH  = max(dot(N, H), 0.0);
          float NdotH2 = NdotH * NdotH;

          float denom  = NdotH2 * (a2 - 1.0) + 1.0;
          return a2 / (PI * denom * denom);
      }

      // Schlick-GGX geometry term for a single direction
      float GeometrySchlickGGX(float NdotV, float roughness)
      {
          // k = (roughness+1)^2 / 8  for direct lighting (remapping for IBL would use k = a^2/2)
          float r = roughness + 1.0;
          float k = (r * r) / 8.0;
          return NdotV / (NdotV * (1.0 - k) + k);
      }

      // Smith geometry: G_sub(N,V) * G_sub(N,L)
      float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
      {
          float NdotV = max(dot(N, V), 0.0);
          float NdotL = max(dot(N, L), 0.0);
          return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
      }

      // Schlick Fresnel
      vec3 FresnelSchlick(float cosTheta, vec3 F0)
      {
          return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
      }

      void main()
      {
          // ©¤©¤ Sample base color ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          vec4 baseColor = texture(baseColorSampler, fragTexCoord) * material.baseColorFactor;

          // ©¤©¤ Alpha test ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          if (material.alphaMode == 1) // MASK
          {
              if (baseColor.a < material.alphaCutoff)
                  discard;
          }

          // ©¤©¤ Reconstruct TBN & perturb normal ©¤©¤©¤©¤©¤©¤©¤©¤
          vec3 N = normalize(fragNormal);
          vec3 T = normalize(fragTangent.xyz);
          T = normalize(T - dot(T, N) * N);
          vec3 B = cross(N, T) * fragTangent.w;          // w = handedness sign (glTF spec)

          vec3 tn = texture(normalSampler, fragTexCoord).xyz * 2.0 - 1.0;
          tn.xy *= material.normalScale;
          mat3 TBN = mat3(T, B, N);
          N = normalize(TBN * tn);

          // ©¤©¤ Metallic & Roughness (glTF channels) ©¤©¤©¤©¤
          // glTF metal-rough: B = metallic, G = roughness
          vec3  mr        = texture(metallicRoughnessSampler, fragTexCoord).rgb;
          float metallic  = mr.b * material.metallicFactor;
          float roughness = mr.g * material.roughnessFactor;
          roughness = max(roughness, 0.001);             // avoid 0-roughness mirror artifacts

          // ©¤©¤ PBR parameters ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          // F0: reflectance at normal incidence
          // dielectrics ¡Ö 0.04, metals take baseColor as F0
          vec3 F0          = mix(vec3(0.04), baseColor.rgb, metallic);
          vec3 diffuseColor = baseColor.rgb * (1.0 - metallic);   // metals have near-zero diffuse

          // ©¤©¤ Lighting vectors ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          vec3 V = normalize(frame.cameraPosition.xyz - fragWorldPos);
          vec3 L = normalize(frame.sunDirection.xyz);
          vec3 H = normalize(V + L);

          vec3 radiance = frame.sunColor.rgb * frame.sunDirection.w;

          // ©¤©¤ Cook-Torrance BRDF ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          // Specular: (D * G * F) / (4 * NdotV * NdotL)
          float D = DistributionGGX(N, H, roughness);
          float G = GeometrySmith(N, V, L, roughness);
          vec3  F = FresnelSchlick(max(dot(H, V), 0.0), F0);

          vec3 numerator   = D * G * F;
          float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
          vec3 specular = numerator / denominator;

          // Diffuse: energy-conserving Lambert
          // kD = (1 - metallic) * (1 - F) ¡ª metals absorb diffuse
          vec3 kD = (1.0 - F) * (1.0 - metallic);
          vec3 diffuse = kD * diffuseColor / PI;

          // ©¤©¤ Direct light contribution ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          float NdotL = max(dot(N, L), 0.0);
          vec3 Lo = (diffuse + specular) * radiance * NdotL;

          // ©¤©¤ Ambient (simple fill, no IBL yet) ©¤©¤©¤©¤©¤©¤©¤
          // Scales with metallic: metals get a specular-tinted ambient
          vec3 ambientDiffuse  = diffuseColor * frame.ambientColor.rgb;
          vec3 ambientSpecular = F0 * frame.ambientColor.rgb * 0.25;  // subdued reflection tint
          vec3 ambient = mix(ambientDiffuse, ambientSpecular, metallic) * frame.ambientColor.a;

          // ©¤©¤ Occlusion ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          float ao = texture(occlusionSampler, fragTexCoord).r;
          ao = mix(1.0, ao, material.occlusionStrength);

          // ©¤©¤ Emissive ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          vec3 emissive = texture(emissiveSampler, fragTexCoord).rgb
                        * material.emissiveFactor.rgb
                        * material.emissiveStrength;

          // ©¤©¤ Final color (linear HDR) ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
          vec3 color = ambient * ao + Lo * ao + emissive;

          // alphaMode == 2 ¡ú BLEND, alpha handled by hardware blend state
          outColor = vec4(color, baseColor.a);
      }