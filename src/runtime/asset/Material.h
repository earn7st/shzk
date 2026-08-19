#pragma once

#include "Asset.h"
#include "runtime/log/Log.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <string>
#include <memory>
#include <array>
#include <glm/glm.hpp>

namespace shzk
{
    class Texture;
    class Shader;

    enum PassMaskBits : uint32_t
    {
        PASS_MASK_NONE                  = 0,
        PASS_MASK_FORWARD_PASS          = 1 << 0,
        // PASS_MASK_DEFERRED_PASS      = 1 << 1,
        // PASS_MASK_TRANSPARENT_PASS   = 1 << 2,
        // PASS_MASK_DEPTH_PASS         = 1 << 3,
        // PASS_MASK_POST_PROCESS_PASS  = 1 << 4,

        PASS_MASK_MAX_ENUM = 0x7FFFFFFF,
    };
    typedef uint32_t PassMask;

    class Material : public Asset
    {
    public:
        Material() : Asset(AssetType::Material) {}
        ~Material() = default;

        // Data
        inline void SetName(const std::string& name) { m_name = name; }
        inline void SetBaseColor(const glm::vec4& color) { m_baseColor = color; }
        inline void SetEmission(const glm::vec3& emmision) { m_emission = emmision; }
        inline void SetMetallic(float value) { m_metallic = value; }
        inline void SetRoughness(float value) { m_roughness = value; }
        inline void SetAlphaCutoff(float value) { m_alphaCutoff = value; }
        inline void SetUseVertexColor(bool value) { m_bUseVertexColor = value; }
        inline void SetTextureDiffuse(const std::shared_ptr<Texture>& texture) { m_textureDiffuse = texture; }
        inline void SetTextureNormal(const std::shared_ptr<Texture>& texture) { m_textureNormal = texture; }
        inline void SetTextureArm(const std::shared_ptr<Texture>& texture) { m_textureArm = texture; }
        inline void SetTextureSpecular(const std::shared_ptr<Texture>& texture) { m_textureSpecular = texture; }
        inline void SetIntSlot(uint8_t idx, int32_t value)
        {
            if (idx >= m_ints.size())
            {
                SHZK_LOG_ERROR("Exceeded Int slots size!");
                return;
            }   
            m_ints[idx] = value;
        }
        inline void SetFloatSlot(uint8_t idx, float value)
        {
            if (idx >= m_floats.size())
            {
                SHZK_LOG_ERROR("Exceeded Float slots size!");
                return;
            }
            m_floats[idx] = value;
        }
        inline void SetColorSlot(uint8_t idx, glm::vec4 color)
        {
            if (idx >= m_colors.size())
            {
                SHZK_LOG_ERROR("Exceeded Color slots size!");
                return;
            }
            m_colors[idx] = color;
        }
        inline void SetTexture2DSlot(uint8_t idx, const std::shared_ptr<Texture>& texture)
        {
            if (idx >= m_texture2D.size())
            {
                SHZK_LOG_ERROR("Exceeded Texture2D slots size!");
                return;
            }
            m_texture2D[idx] = texture;
        }
        inline void SetTextureCubeSlot(uint8_t idx, const std::shared_ptr<Texture>& texture)
        {
            if (idx >= m_textureCube.size())
            {
                SHZK_LOG_ERROR("Exceeded TextureCube slots size!");
                return;
            }
            m_textureCube[idx] = texture;
        }
        inline void SetTexture3DSlot(uint8_t idx, const std::shared_ptr<Texture>& texture)
        {
            if (idx >= m_texture3D.size())
            {
                SHZK_LOG_ERROR("Exceeded Texture3D slots size!");
                return;
            }
            m_texture3D[idx] = texture;
        }

        inline void SetVertexShader(const std::shared_ptr<Shader>& shader) { m_vertexShader = shader; };
        inline void SetFragmentShader(const std::shared_ptr<Shader>& shader) { m_fragmentShader = shader; };

        inline std::string GetName() const { return m_name; }
        inline glm::vec4 GetBaseColor() { return m_baseColor; }
        inline glm::vec3 GetEmission() { return m_emission; }
        inline float GetMetallic() { return m_metallic; }
        inline float GetRoughness() { return m_roughness; }
        inline float GetAlphaCutoff() { return m_alphaCutoff; }
        inline bool UseVertexColor() { return m_bUseVertexColor; }
        inline std::shared_ptr<Texture> GetTextureDiffuse() const { return m_textureDiffuse; }
        inline std::shared_ptr<Texture> GetTextureNormal() const { return m_textureNormal; }
        inline std::shared_ptr<Texture> GetTextureArm() const { return m_textureArm; }
        inline std::shared_ptr<Texture> GetTextureSpecular() const { return m_textureSpecular; }
        // TODO: General Slots
        inline std::shared_ptr<Shader> GetVertexShader() const { return m_vertexShader; }
        inline std::shared_ptr<Shader> GetFragmentShader() const { return m_fragmentShader; }

        // Pipeline
        inline void SetPassMask(uint32_t mask) { m_passMask = mask; }
        inline void SetRasterizerCullMode(RasterizerCullMode mode) { m_cullMode = mode; }
        inline void SetRasterizerFillMode(RasterizerFillMode mode) { m_fillMode = mode; }

        inline uint32_t GetPassMask() const { return m_passMask; }
        inline RasterizerCullMode GetRasterizerCullMode() const { return m_cullMode; }
        inline RasterizerFillMode GetRasterizerFillMode() const { return m_fillMode; }
        inline bool DepthTest() const { return m_bDepthTest; }
        inline bool DepthWrite() const { return m_bDepthWrite; }
        inline CompareFunction GetDepthCompare() const{ return m_depthCompare; }
        inline bool CastShadow() const { return m_bCastShadow; }

    protected:
        std::string m_name;

        // Parameters
        glm::vec4 m_baseColor = glm::vec4(1.f);
        glm::vec3 m_emission = glm::vec3(0.f);
        float m_metallic = 1.0f;
        float m_roughness = 1.0f;
        float m_alphaCutoff = 0.5f;
        bool m_bUseVertexColor = false;

        std::shared_ptr<Texture> m_textureDiffuse;
        std::shared_ptr<Texture> m_textureNormal;
        std::shared_ptr<Texture> m_textureArm;
        std::shared_ptr<Texture> m_textureSpecular;

        // General slots
        std::array<int32_t, 8> m_ints{};    // 0: alpha mode 1£ºdouble sided 2: unlit
        std::array<float, 8>   m_floats{};  // 0: normal scale 1: occlusion strength
        std::array<glm::vec4, 8> m_colors{};
        std::array<std::shared_ptr<Texture>, 8> m_texture2D;    // 0: occlusion 1: emissive
        std::array<std::shared_ptr<Texture>, 4> m_textureCube;
        std::array<std::shared_ptr<Texture>, 4> m_texture3D;

        std::shared_ptr<Shader> m_vertexShader      = nullptr;  // use render passes's default shader
                                                                // TODO: user defined shaders
        std::shared_ptr<Shader> m_fragmentShader    = nullptr;

    protected:
        PassMask m_passMask = PASS_MASK_FORWARD_PASS;   

        // RasteriazerState
        RasterizerCullMode m_cullMode = RasterizerCullMode::CW;
        RasterizerFillMode m_fillMode = RasterizerFillMode::Solid;
        
        // DepthStencilState
        bool m_bDepthTest    = true;
        bool m_bDepthWrite   = true;
        CompareFunction m_depthCompare = CompareFunction::LessEqual;    

        //bool m_bUseForDepthPass = true; 
        bool m_bCastShadow = true;
		
        // TODO: MaterialData data 
	};
}