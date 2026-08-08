#pragma once

#include "Asset.h"

#include <string>
#include <memory>
#include <array>
#include <glm/glm.hpp>

namespace shzk
{
    class Texture;
    class Shader;

	class Material : public Asset
	{
	public:
		Material() : Asset(AssetType::Material) {}
		~Material() = default;

        // void SetVertexShader();
        // void SetFragmentShader();

	public:
        std::string m_name;

        // Parameters
        glm::vec4 m_baseColor = glm::vec4(1.f);
        glm::vec3 m_emission = glm::vec3(0.f);
        float m_metallic = 1.0f;
        float m_roughness = 1.0f;
        float m_alphaCutoff = 0.5f;
        bool m_useVertexColor = false;

        std::shared_ptr<Texture> m_textureDiffuse;
        std::shared_ptr<Texture> m_textureNormal;
        std::shared_ptr<Texture> m_textureArm;
        std::shared_ptr<Texture> m_textureSpecular;

        // General slots
        std::array<int32_t, 8> m_ints{};
        std::array<float, 8>   m_floats{};
        std::array<glm::vec4, 8> m_colors{};
        std::array<std::shared_ptr<Texture>, 8> m_texture2D;
        std::array<std::shared_ptr<Texture>, 4> m_textureCube;
        std::array<std::shared_ptr<Texture>, 4> m_texture3D;

        std::shared_ptr<Shader> m_vertexShader      = nullptr;  // use render passes's default shader
                                                                // TODO: user defined shaders
        std::shared_ptr<Shader> m_fragmentShader    = nullptr;
		
	};
}