#pragma once

#include "Asset.h"
#include "runtime/rhi/RHIResource.h"

#include <string>

namespace shzk
{
    class Texture : public Asset
    {
    public:
        Texture() = default;
        ~Texture();

        inline const std::string& GetName() const { return m_name; }

    private:
        std::string m_name;

        std::shared_ptr<RHITexture> m_texture;
    };
}