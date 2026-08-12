#pragma once

#include <memory>

namespace shzk
{
	class RHIGraphicsPipeline;

	class GraphicsPipelineState
	{
	public:
		GraphicsPipelineState() = default;
		~GraphicsPipelineState() = default;

	private:
		std::shared_ptr<RHIGraphicsPipeline> m_rhiPipeline;
	};
}