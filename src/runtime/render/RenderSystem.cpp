#include "RenderSystem.h"

#include "runtime/rhi/RHICommandList.h"
#include "runtime/rhi/RHIDevice.h"
#include "runtime/rhi/RHIQueue.h"
#include "runtime/rhi/RHISwapchain.h"

#include <memory>

namespace vkR
{
	void RenderSystem::Initialize(const RenderSystemInitInfo& initInfo)
	{
		rhi::RHIInitInfo rhiInitInfo;
		rhiInitInfo.windowSystem = initInfo.windowSystem;

		m_device = std::make_shared<rhi::Device>();
		m_device->Initialize(rhiInitInfo);

		m_swapchain = m_device->GetSwapchain();
	}

	void RenderSystem::Tick()
	{ 
		m_currentCmdListGraphics = m_device->GetGraphicsQueue()->NextCommandList();
		m_swapchain->AcquireNextImage();
		if (!m_swapchain->IsImageAcquired()) return;
		m_currentCmdListGraphics->Begin();

		ProduceFrame();

		SubmitAndPresent();
	}

	void RenderSystem::ProduceFrame()
	{
		m_currentCmdListGraphics->ImageBarrier(
			m_swapchain->GetCurrentImage(),
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT
		);

		m_currentCmdListGraphics->BeginRendering(
			m_swapchain->GetCurrentImageView(),
			m_swapchain->GetExtent(),
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			{ 0.7f, 0.0f, 0.7f, 1.0f }
		);

	}

	void RenderSystem::SubmitAndPresent()
	{
		if (m_swapchain->IsImageAcquired())
		{
			m_currentCmdListGraphics->EndRendering();
			m_currentCmdListGraphics->ImageBarrier(
				/* image        */ m_swapchain->GetCurrentImage(),
				/* oldLayout    */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				/* newLayout    */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				/* srcStage     */ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
				/* srcAccess    */ VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
				/* dstStage     */ VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
				/* dstAccess    */ VK_ACCESS_2_NONE,
				/* aspect       */ VK_IMAGE_ASPECT_COLOR_BIT
			);

			m_currentCmdListGraphics->Submit(
				m_swapchain->GetCurrentImageAcquiredSemaphore(),
				m_swapchain->GetCurrenRenderCompleteSemaphore()
			);
			m_swapchain->Present(m_currentCmdListGraphics);
		}
	}

	void RenderSystem::Shutdown()
	{ 
		m_swapchain = nullptr;
		m_device->Shutdown();
	}
}