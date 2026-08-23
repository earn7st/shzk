#pragma once

#include "runtime/rhi/RHIResource.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <memory>
#include <unordered_map>

namespace shzk
{
	class RHIGraphicsPipeline;

	class GraphicsPipelineCache
	{
	public:
		GraphicsPipelineCache() = default;
		~GraphicsPipelineCache() = default;
		
		std::shared_ptr<RHIGraphicsPipeline> GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineInfo& info);

		// TODO: unordered_map and abstraction of GraphicsPipelineState
		/*struct Key
		{
			RHIGraphicsPipelineInfo info;
			Key(RHIGraphicsPipelineInfo info)
				: info(info) {}

			friend bool operator==(const Key& a, const Key& b)
			{
				return a.info == b.info;
			}
		};*/

		//struct GraphicsPipelineState // Currently a thin wrapper
		//{
		//	std::shared_ptr<RHIGraphicsPipeline> pipeline;
		//};

	private:
		std::vector<std::pair<RHIGraphicsPipelineInfo, std::shared_ptr<RHIGraphicsPipeline>>> m_cache;
	};
}