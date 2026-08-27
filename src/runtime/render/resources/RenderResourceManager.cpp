#include "RenderResourceManager.h"
#include "runtime/log/Log.h"
#include "runtime/render/RenderConfig.h"
#include "runtime/render/resources/Sampler.h"
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

    void RenderResourceManager::BeginFrame(uint32_t frameIdx)
    {
        m_frameIndex = frameIdx;
    }

    std::shared_ptr<RHIDescriptorSet> RenderResourceManager::CreateMaterialDescriptorSet()
    {
        return m_materialRootSignature->CreateDescriptorSet(DESCRIPTORSET_INDEX_MATERIAL);
    }

    void RenderResourceManager::InitGlobalResources()
    {
        // per frame root signature
        {
            RHIRootSignatureInfo perFrameInfo{};
            perFrameInfo.AddEntry({ .set = DESCRIPTORSET_INDEX_PER_FRAME, .binding = PER_FRAME_BINDING_VIEW, .size = 1, .frequency = SHADER_FREQUENCY_ALL, .type = RESOURCE_TYPE_UNIFORM_BUFFER });
            m_perFrameRootSignature = RHI::Get()->CreateRootSignature(perFrameInfo);
        }
        
        // material root signature
        {
            RHIRootSignatureInfo matInfo{};
            matInfo.AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_UNIFORM, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_UNIFORM_BUFFER })
                .AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_DIFFUSE, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER })
                .AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_NORMAL, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER })
                .AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_ARM, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER })
                .AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_SPECULAR, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
            for (int i = 0; i < 8; ++i)
                matInfo.AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_TEXTURE2D + i, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
            for (int i = 0; i < 4; ++i)
                matInfo.AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_TEXTURECUBE + i, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
            for (int i = 0; i < 4; ++i)
                matInfo.AddEntry({ .set = DESCRIPTORSET_INDEX_MATERIAL, .binding = MATERIAL_BINDING_TEXTURE3D + i, .size = 1, .frequency = SHADER_FREQUENCY_FRAGMENT, .type = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER });
            m_materialRootSignature = RHI::Get()->CreateRootSignature(matInfo);
        }
        
        // per frame resources
        {
            for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
            {
                auto& perFrame = m_perFrameResources[i];
                // descriptor set (set 0)
                perFrame.descriptorSet = m_perFrameRootSignature->CreateDescriptorSet(DESCRIPTORSET_INDEX_PER_FRAME);
                perFrame.ub = std::make_shared<Buffer<PerFrameUniformShaderParameters>>();
                perFrame.descriptorSet->UpdateBuffer(PER_FRAME_BINDING_VIEW, perFrame.ub->GetBuffer());

                // color attachment
                RHITextureInfo info{};
                info.format = HDR_COLOR_FORMAT;
                info.extent = { m_renderExtent.width, m_renderExtent.height, 1 };
                info.arrayLayers = 1;
                info.mipLevels = 1;
                info.memoryUsage = MemoryUsage::GPUOnly;
                info.type = RESOURCE_TYPE_TEXTURE | RESOURCE_TYPE_RENDER_TARGET;
                perFrame.sceneColorTexture = RHI::Get()->CreateTexture(info);

                RHITextureViewInfo viewInfo{};
                viewInfo.texture = perFrame.sceneColorTexture;
                viewInfo.format = HDR_COLOR_FORMAT;
                viewInfo.viewType = TextureViewType::View2D;
                perFrame.sceneColorTextureView = RHI::Get()->CreateTextureView(viewInfo);
            }
        }

        // multiframe resources
        {
            // default sampler
            m_samplers.push_back(std::make_shared<Sampler>(
                FilterType::Linear,
                SamplerMipmapMode::Linear,
                SamplerAddressMode::Repeat,
                0.f));
        }
        
    }
}