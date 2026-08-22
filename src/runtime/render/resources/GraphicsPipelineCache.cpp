#include "GraphicsPipelineCache.h"

#include "runtime/rhi/RHI.h"

namespace shzk
{
	std::shared_ptr<RHIGraphicsPipeline> GraphicsPipelineCache::GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineInfo& info)
	{
		for (const auto& [key, pipeline] : m_cache)
		{
			if (key == info) return pipeline;
		}

		auto pipeline = RHI::Get()->CreateGraphicsPipeline(info);
		m_cache.emplace_back(info, pipeline);
		return pipeline;
	}
}