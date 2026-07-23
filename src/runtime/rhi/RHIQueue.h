#pragma once

#include "runtime/rhi/RHIDefinitions.h"

namespace shzk
{
	class RHIQueue
	{
	public:
		RHIQueue(RHIQueueInfo& info) : m_info(info) {}
		~RHIQueue() = default;

		inline RHIQueueType GetType() const { return m_type; }

	protected:
		RHIQueueInfo m_info;
	};
}