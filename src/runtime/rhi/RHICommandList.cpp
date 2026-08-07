#include "RHICommandList.h"

#include "runtime/log/Log.h"
#include "runtime/rhi/RHI.h"

#include <memory>

#include <cassert>

namespace shzk
{
	std::shared_ptr<RHICommandList> RHICommandList::g_rhiCmdList = std::make_shared<RHICommandList>();
	
	void RHICommandList::Init(bool bypass)
	{
		g_rhiCmdList->m_bypass = bypass;
	}

	void RHICommandList::BeginCommand()
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIBeginCommand();
		}
	}

	void RHICommandList::EndCommand()
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIEndCommand();
		}
	}

	void RHICommandList::Submit(
		std::shared_ptr<RHIFence> fence,
		std::shared_ptr<RHISemaphore> waitSemaphore,
		std::shared_ptr<RHISemaphore> signalSemaphore)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHISubmit(fence, waitSemaphore, signalSemaphore);
		}
	}

	void RHICommandList::TextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHITextureClearColor(texture, rgba);
		}
	}

	void RHICommandList::TextureBarrier(const RHITextureBarrier& barrier)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHITextureBarrierCommand(barrier);
		}
	}

	// void Submit(std::shared_ptr<RHISemaphore> wait,
	//	std::shared_ptr<RHISemaphore> signal,
	//	std::shared_ptr<RHIFence> fence) = 0;
	// void DrawIndexed() = 0;
	// void SetGraphicsPipeline() = 0;
	// void BindVertexBuffer() = 0;
}