#pragma once

#include "runtime/rhi/RHIDefinitions.h"

namespace shzk
{
	class RHISampler;

	class Sampler
	{
	public:
		Sampler();
		Sampler(FilterType filter = FilterType::Linear, 
				SamplerMipmapMode mipmapMode = SamplerMipmapMode::Linear, 
				SamplerAddressMode addressMode = SamplerAddressMode::Repeat,
				float maxAnisotropy = 0.f);
		~Sampler() = default;

		std::shared_ptr<RHISampler> m_sampler;
	};
}