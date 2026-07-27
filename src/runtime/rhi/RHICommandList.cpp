#include "RHICommandList.h"

#include "runtime/log/Log.h"

#include <memory>

namespace shzk
{
	std::shared_ptr<RHICommandList> RHICommandList::g_RhiCmdList = std::make_shared<RHICommandList>();
	
	void RHICommandList::Init(bool bypass)
	{
		g_RhiCmdList->m_bypass = bypass;
	}

	void RHICommandList::Begin()
	{
		if (!m_cmdContext)
		{
			SHZK_LOG_WARN("RHI command context is empty, cannot Begin()");
			return;
		}
	}

	void RHICommandList::End()
	{
		if (!m_cmdContext)
		{
			SHZK_LOG_WARN("RHI command context is empty, cannot End()");
			return;
		}

	}

	// void Submit(std::shared_ptr<RHISemaphore> wait,
	//	std::shared_ptr<RHISemaphore> signal,
	//	std::shared_ptr<RHIFence> fence) = 0;
	// void DrawIndexed() = 0;
	// void SetGraphicsPipeline() = 0;
	// void BindVertexBuffer() = 0;
}