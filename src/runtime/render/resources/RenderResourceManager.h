#pragma once

#include "runtime/rhi/RHIDefinitions.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace shzk
{
	enum PerFrameBinding : uint32_t
	{

	};

	enum MaterialBinding : uint32_t
	{
		MATERIAL_BINDING_UNIFORM = 0,   // uniform buffer: Material Data
		MATERIAL_BINDING_DIFFUSE = 1,
		MATERIAL_BINDING_NORMAL = 2,
		MATERIAL_BINDING_ARM = 3,
		MATERIAL_BINDING_SPECULAR = 4,
		MATERIAL_BINDING_TEXTURE2D = 5,   // [5, 13)  m_texture2D[8]
		MATERIAL_BINDING_TEXTURECUBE = 13,  // [13, 17) m_textureCube[4]
		MATERIAL_BINDING_TEXTURE3D = 17,  // [17, 21) m_texture3D[4]
	};

	class GraphicsPipelineState;
	class RHIShader;

	class RenderResourceManager
	{
	public:
		static std::shared_ptr<RenderResourceManager> g_renderResourceManager;

	public:
		RenderResourceManager() = default;
		~RenderResourceManager() = default;

		static std::shared_ptr<RenderResourceManager>& Get() { return g_renderResourceManager; }

		void Init();

		std::shared_ptr<RHIShader> GetOrCreateRHIShader(const std::string& path, ShaderFrequency frequency, const std::string& entry = "main");

	private:
		void InitGlobalResources();

		std::unordered_map<std::string, std::shared_ptr<RHIShader>> m_rhiShaderMap;

		struct PerFrameResource
		{
			std::shared_ptr<RHIDescriptorSet> descriptorSet;

			// Uniform Buffers: Camera, Light
		};
		std::array<PerFrameResource, FRAMES_IN_FLIGHT> m_perFrameResources;

		std::shared_ptr<RHIRootSignature> m_perFrameRootSignature;
		std::shared_ptr<RHIRootSignature> m_materialRootSignature;
	};
}