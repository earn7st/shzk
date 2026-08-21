#pragma once

namespace shzk
{
	typedef struct MaterialUniformData
	{
        glm::vec4 baseColor;
        glm::vec3 emission;

        float metallic;
        float roughness;
        float alphaCutoff;
        uint32_t bUseVertexColor;

        // General slots
        std::array<int32_t, 8> m_ints{};    // 0: alpha mode 1£ºdouble sided 2: unlit
        std::array<float, 8>   m_floats{};  // 0: normal scale 1: occlusion strength
        std::array<glm::vec4, 8> m_colors{};
	};
}