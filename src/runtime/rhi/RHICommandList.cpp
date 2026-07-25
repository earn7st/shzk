#include "RHICommandList.h"

#include <memory>

namespace shzk
{
	std::shared_ptr<RHICommandList> g_RhiCmdList = std::make_shared<RHICommandList>();

	void RHICommandList::Begin()
	{

	}

	void RHICommandList::End()
	{

	}

	// void Submit(std::shared_ptr<RHISemaphore> wait,
	//	std::shared_ptr<RHISemaphore> signal,
	//	std::shared_ptr<RHIFence> fence) = 0;
	// void DrawIndexed() = 0;
	// void SetGraphicsPipeline() = 0;
	// void BindVertexBuffer() = 0;
}