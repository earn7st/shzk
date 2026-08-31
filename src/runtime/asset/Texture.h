#pragma once

#include "Asset.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <string>

namespace shzk
{
    enum class TextureType
    {
        Type2D,
        Type2DArray,
        TypeCube,
        Type3D,
        
        Max,
    };

    TextureViewType TextureTypeToTextureViewType(TextureType type);

    class Texture : public Asset
    {
    public:
        Texture() = delete;
        Texture(std::string path, TextureType type, RHIFormat format = RHIFormat::FORMAT_R8G8B8A8_SRGB);
        Texture(Extent2D extent, glm::vec4 rgba);
        ~Texture() = default;

    public:
        TextureType m_type = TextureType::Max;
        std::vector<std::string> m_paths;

        Extent3D    m_extent{};
        uint32_t    m_mipLevels = 1;
        uint32_t    m_arrayLayer = 1;
        RHIFormat   m_format = FORMAT_UKNOWN;
        std::shared_ptr<RHITexture> m_texture;
        std::shared_ptr<RHITextureView> m_textureView;
        bool m_rhiInitialized = false;

    private:
        void LoadFromFile();
        void InitRHI();
    };
}