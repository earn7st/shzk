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
        Texture(std::string path, TextureType type);
        ~Texture() = default;

        inline void SetName(std::string name) { m_name = name; }
        inline const std::string& GetName() const { return m_name; }

    private:
        void LoadFromFile();
        void InitRHI();

    private:    
        TextureType m_type = TextureType::Max;
        std::string m_name = "Untitled";
        std::vector<std::string> m_paths;

        Extent3D    m_extent{};
        uint32_t    m_mipLevels = 1;
        uint32_t    m_arrayLayer = 1;
        RHIFormat   m_format = FORMAT_UKNOWN;
        std::shared_ptr<RHITexture> m_texture;
        std::shared_ptr<RHITextureView> m_textureView;
        bool m_rhiInitialized = false;
    };
}