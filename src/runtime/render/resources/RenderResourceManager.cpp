#include "RenderResourceManager.h"
#include "runtime/log/Log.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIResource.h"
#include "runtime/rhi/RHIDefinitions.h"


#include <memory>
#include <fstream>

namespace shzk
{
	std::shared_ptr<RenderResourceManager> RenderResourceManager::g_renderResourceManager = std::make_shared<RenderResourceManager>();

	void RenderResourceManager::Init()
	{
        InitGlobalResources();
	}

	std::shared_ptr<RHIShader> RenderResourceManager::GetOrCreateRHIShader(const std::string& path, ShaderFrequency frequency, const std::string& entry)
	{
		auto iter = m_rhiShaderMap.find(path);
		if (iter != m_rhiShaderMap.end()) return iter->second;

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            SHZK_LOG_ERROR("Failed to open shader file: {}", path);
            return nullptr;
        }

        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> code(static_cast<size_t>(fileSize));
        if (!code.empty())
            file.read(reinterpret_cast<char*>(code.data()), fileSize);

        RHIShaderInfo shaderInfo = {
            .entry = entry,
            .frequency = frequency,
            .code = std::move(code),
        };

        std::shared_ptr<RHIShader> shader = RHI::Get()->CreateShader(shaderInfo);

        m_rhiShaderMap.emplace(path, shader);
        return shader;
	}

    void RenderResourceManager::InitGlobalResources()
    {
        // per frame root signature
        RHIRootSignatureInfo perFrameInfo{};


        // material root signature
        RHIRootSignatureInfo matInfo{};
        matInfo.AddEntry({ .set = 1, .binding = MATERIAL_BINDING_UNIFORM, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_UNIFORM_BUFFER })
            .AddEntry({ .set = 1, .binding = MATERIAL_BINDING_DIFFUSE, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER })
            .AddEntry({ .set = 1, .binding = MATERIAL_BINDING_NORMAL, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER })
            .AddEntry({ .set = 1, .binding = MATERIAL_BINDING_ARM, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER })
            .AddEntry({ .set = 1, .binding = MATERIAL_BINDING_SPECULAR, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
        for (int i = 0; i < 8; ++i)
            matInfo.AddEntry({ .set = 1, .binding = MATERIAL_BINDING_TEXTURE2D + i, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
        for (int i = 0; i < 4; ++i)
            matInfo.AddEntry({ .set = 1, .binding = MATERIAL_BINDING_TEXTURECUBE + i, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
        for (int i = 0; i < 4; ++i)
            matInfo.AddEntry({ .set = 1, .binding = MATERIAL_BINDING_TEXTURE3D + i, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
        m_materialRootSignature = RHI::Get()->CreateRootSignature(matInfo);
    }

}