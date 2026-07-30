#pragma once

#include "RHIDefinitions.h"

namespace shzk
{
	class RHIResource
	{
	public:
		RHIResource() = delete;
		RHIResource(RHIResourceType resourceType) : m_resourceType(resourceType) {}
		~RHIResource() = default;

		inline RHIResourceType GetType() { return m_resourceType; }

	protected:
		virtual void Destroy() = 0;

	protected:
		RHIResourceType m_resourceType;
	};

	class RHIBuffer : public RHIResource
	{
	public:
		RHIBuffer() = delete;
		~RHIBuffer() = default;

	protected:

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

	protected:
		RHITextureInfo m_info;

		TextureSubresourceRange m_defaultRange;
		TextureSubresourceLayers m_defaultLayers;
	};
}