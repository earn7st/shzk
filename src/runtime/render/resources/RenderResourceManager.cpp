#include "RenderResourceManager.h"
#include "runtime/log/Log.h"
#include "runtime/rhi/RHI.h"
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
        // per frame RHIRootSignature
        RHIRootSignatureInfo info{};
        // TODO
        // per frame resources

    }

}