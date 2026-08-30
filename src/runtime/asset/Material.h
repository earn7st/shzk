#pragma once

#include "Asset.h"
#include "runtime/log/Log.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/asset/Texture.h"
#include "runtime/render/resources/RenderResourceDefinitions.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <string>
#include <memory>
#include <array>
#include <glm/glm.hpp>

namespace shzk
{
    class Texture;
    class Shader;
    class RHIRootSignature;
    class RHIDescriptorSet;

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

        void InitRenderResources();
        void UpdateUniformData();
        void UpdateTexture(uint32_t binding, std::shared_ptr<Texture> texture);

        // Getter & Setters
        inline void SetName(const std::string& name)        { m_name = name; }
        inline void SetBaseColor(const glm::vec4& color)    { m_baseColor = color;      UpdateUniformData(); }
        inline void SetEmission(const glm::vec3& emmision)  { m_emission = emmision;    UpdateUniformData(); }
        inline void SetMetallic(float value)                { m_metallic = value;       UpdateUniformData();}
        inline void SetRoughness(float value)               { m_roughness = value;      UpdateUniformData();}
        inline void SetAlphaCutoff(float value)             { m_alphaCutoff = value;    UpdateUniformData(); }
        inline void SetUseVertexColor(bool value)           { m_bUseVertexColor = value; UpdateUniformData(); }
        inline void SetTextureDiffuse(const std::shared_ptr<Texture>& texture)  
        { 
            if (!texture) return;
            m_textureDiffuse = texture;   
            UpdateTexture(MATERIAL_BINDING_DIFFUSE, m_textureDiffuse); 
        }
        inline void SetTextureNormal(const std::shared_ptr<Texture>& texture)   
        { 
            if (!texture) return;
            m_textureNormal = texture;    
            UpdateTexture(MATERIAL_BINDING_NORMAL, m_textureNormal); 
        }
        inline void SetTextureArm(const std::shared_ptr<Texture>& texture)      
        { 
            if (!texture) return;
            m_textureArm = texture;       
            UpdateTexture(MATERIAL_BINDING_ARM, m_textureArm); 
        }
        inline void SetTextureSpecular(const std::shared_ptr<Texture>& texture) 
        { 
            if (!texture) return;
            m_textureSpecular = texture;  
            UpdateTexture(MATERIAL_BINDING_SPECULAR, m_textureSpecular); 
        }
        inline void SetIntSlot(uint8_t idx, int32_t value)
        {
            if (idx >= m_ints.size())
            {
                SHZK_LOG_ERROR("Exceeded Int slots size!");
                return;
            }   
            m_ints[idx] = value;
            UpdateUniformData();
        }
        inline void SetFloatSlot(uint8_t idx, float value)
        {
            if (idx >= m_floats.size())
            {
                SHZK_LOG_ERROR("Exceeded Float slots size!");
                return;
            }
            m_floats[idx] = value;
            UpdateUniformData();
        }
        inline void SetColorSlot(uint8_t idx, glm::vec4 color)
        {
            if (idx >= m_colors.size())
            {
                SHZK_LOG_ERROR("Exceeded Color slots size!");
                return;
            }
            m_colors[idx] = color;
            UpdateUniformData();
        }
        inline void SetTexture2DSlot(uint8_t idx, const std::shared_ptr<Texture>& texture)
        {
            if (!texture) return;
            if (idx >= m_texture2D.size())
            {
                SHZK_LOG_ERROR("Exceeded Texture2D slots size!");
                return;
            }
            m_texture2D[idx] = texture;
            UpdateTexture(MATERIAL_BINDING_TEXTURE2D + idx, m_texture2D[idx]);
        }
        inline void SetTextureCubeSlot(uint8_t idx, const std::shared_ptr<Texture>& texture)
        {
            if (!texture) return;
            if (idx >= m_textureCube.size())
            {
                SHZK_LOG_ERROR("Exceeded TextureCube slots size!");
                return;
            }
            m_textureCube[idx] = texture;
            UpdateTexture(MATERIAL_BINDING_TEXTURECUBE + idx, m_textureCube[idx]);
        }
        inline void SetTexture3DSlot(uint8_t idx, const std::shared_ptr<Texture>& texture)
        {
            if (!texture) return;
            if (idx >= m_texture3D.size())
            {
                SHZK_LOG_ERROR("Exceeded Texture3D slots size!");
                return;
            }
            m_texture3D[idx] = texture;
            UpdateTexture(MATERIAL_BINDING_TEXTURE3D + idx, m_texture3D[idx]);
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

        std::shared_ptr<RHIDescriptorSet> GetDescriptorSet() const { return m_descriptorSet; }
    
    // Plain datas and references to textures
    protected:
        std::string m_name;

        std::shared_ptr<Shader> m_vertexShader = nullptr;  // use render passes's default shader
        // TODO: user defined shaders
        std::shared_ptr<Shader> m_fragmentShader = nullptr;

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
        std::array<std::shared_ptr<Texture>, 8> m_texture2D;    // 0: occlusion 1: emissive 2~7: not used
        std::array<std::shared_ptr<Texture>, 4> m_textureCube;  // not used
        std::array<std::shared_ptr<Texture>, 4> m_texture3D;    // not used 

    // CPU side options
    protected:
        PassMask m_passMask = PASS_MASK_FORWARD_PASS;

        // RasteriazerState
        RasterizerCullMode m_cullMode = RasterizerCullMode::CW;
        RasterizerFillMode m_fillMode = RasterizerFillMode::Solid;

        // DepthStencilState
        bool m_bDepthTest = true;
        bool m_bDepthWrite = true;
        CompareFunction m_depthCompare = CompareFunction::GreaterEqual;   // Reverse-Z

        //bool m_bUseForDepthPass = true; 
        bool m_bCastShadow = true;

    // GPU side resources and data for upload
    protected:
        std::shared_ptr<Buffer<MaterialUniformShaderParameters>> m_buffer;

        // The default material set 1
        // Layout should be retrived from RenderResourceManager.m_materialRootSignature
        std::shared_ptr<RHIDescriptorSet> m_descriptorSet;
	};
}