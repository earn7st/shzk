#pragma once

#include "RHIDefinitions.h"

namespace shzk
{
	class RHIResource
	{
	public:
		RHIResource() = delete;
		RHIResource(RHIResourceType resourceType) : m_resourceType(resourceType) {}
		virtual ~RHIResource() = default;

		inline RHIResourceType GetType() const { return m_resourceType; }

	protected:
		virtual void Destroy() = 0;

	protected:
		RHIResourceType m_resourceType;
		uint32_t		m_framesLastUsed = 0;
		friend class RHI;
	};

	class RHIBuffer : public RHIResource
	{
	public:
		RHIBuffer() = delete;
		RHIBuffer(const RHIBufferInfo& info)
			: RHIResource(RHIResourceType::Buffer), m_info(info) {}
		~RHIBuffer() = default;

		inline const RHIBufferInfo& GetInfo() const { return m_info; }

		virtual void* Map() = 0;
		virtual void UnMap() = 0;

	protected:
		RHIBufferInfo m_info;
	};

	class RHITexture : public RHIResource
	{
	public:
		RHITexture() = delete;
		RHITexture(const RHITextureInfo& info)
			: RHIResource(RHIResourceType::Texture), m_info(info) {}
		~RHITexture() = default;

		const TextureSubresourceRange& GetDefaultSubresourceRange() const { return m_defaultRange; }
		const TextureSubresourceLayers& GetDefaultSubresourceLayers() const { return m_defaultLayers; }

		inline const RHITextureInfo& GetInfo() const { return m_info; }

		Extent3D MipExtent(uint32_t mipLevel);

	protected:
		RHITextureInfo m_info;

		TextureSubresourceRange m_defaultRange = {};
		TextureSubresourceLayers m_defaultLayers = {};
	};

	class RHITextureView : public RHIResource
	{
	public:
		RHITextureView() = delete;
		RHITextureView(const RHITextureViewInfo& info)
			: RHIResource(RHIResourceType::TextureView), m_info(info) {}

		inline const RHITextureViewInfo& GetInfo() const { return m_info; }
	
	protected:
		RHITextureViewInfo m_info;
	};

	class RHISampler : public RHIResource
	{
	public:
		RHISampler() = delete;
		RHISampler(const RHISamplerInfo& info)
			: RHIResource(RHIResourceType::Sampler), m_info(info) {}
		
		inline const RHISamplerInfo& GetInfo() const { return m_info; }

	protected:
		RHISamplerInfo m_info;
	};

	class RHIShader : public RHIResource
	{
	public:
		RHIShader() = delete;
		RHIShader(const RHIShaderInfo& info)
			: RHIResource(RHIResourceType::Shader), m_info(info)  {}

		inline const RHIShaderInfo& GetInfo() const { return m_info; }
		inline ShaderFrequency GetFrequency() const { return m_frequency; }

	protected:
		RHIShaderInfo m_info;
	
	private:
		ShaderFrequency m_frequency;
	};

	// DescriptorSet and RootSignature
	class RHIDescriptorSet : public RHIResource
	{
	public:
		RHIDescriptorSet() : RHIResource(RHIResourceType::DescriptorSet) {}

	protected:
		virtual void Destroy() = 0;
	};

	class RHIRootSignature : public RHIResource
	{
	public:
		RHIRootSignature() = delete;
		RHIRootSignature(const RHIRootSignatureInfo& info)
			: RHIResource(RHIResourceType::RootSignature), m_info(info) {}

		virtual std::shared_ptr<RHIDescriptorSet> CreateDescriptorSet() = 0;

		inline const RHIRootSignatureInfo& GetInfo() const { return m_info; }

	private:
		RHIRootSignatureInfo m_info;
	};

	// Pipeline
	class RHIGraphicsPipeline : public RHIResource
	{
	public:
		RHIGraphicsPipeline() = delete;
		RHIGraphicsPipeline(const RHIGraphicsPipelineInfo& info)
			: RHIResource(RHIResourceType::GraphicsPipeline), m_info(info) {}

		inline const RHIGraphicsPipelineInfo& GetInfo() const { return m_info; }

	protected:
		RHIGraphicsPipelineInfo m_info;
	};
}