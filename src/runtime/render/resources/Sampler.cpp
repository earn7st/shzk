#include "Sampler.h"

#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIDefinitions.h"

namespace shzk
{
	Sampler::Sampler()
	{
		RHISamplerInfo info{};	// default RHISamplerInfo 
		m_sampler = RHI::Get()->CreateSampler(info);
	}

	Sampler::Sampler(FilterType filter, SamplerMipmapMode mipmapMode, SamplerAddressMode addressMode, float maxAnisotropy)
	{
		RHISamplerInfo info{};
		info.magFilter = filter;
		info.minFilter = filter;
		info.mipmapMode = mipmapMode;
		info.addressModeU = addressMode;
		info.addressModeV = addressMode;
		info.addressModeW = addressMode;
		info.maxAnisotropy = maxAnisotropy;
		if (maxAnisotropy != 0.f) info.bAnisotropyEnable = true;

		m_sampler = RHI::Get()->CreateSampler(info);
	}

}