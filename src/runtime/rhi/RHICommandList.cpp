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

	void RHICommandList::BlitTexture(std::shared_ptr<RHITexture> src, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers srcSubresource, TextureSubresourceLayers dstSubresource, FilterType filter)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIBlitTexture(src, dst, srcSubresource, dstSubresource, filter);
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

	void RHICommandList::BufferBarrier(const RHIBufferBarrier& barrier)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIBufferBarrierCommand(barrier);
		}
	}

	void RHICommandList::CopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHICopyTextureToBuffer(src, srcSubresource, dst, dstOffset);
		}
	}

	void RHICommandList::CopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHICopyBufferToTexture(src, srcOffset, dst, dstSubresource);
		}
	}

	void RHICommandList::CopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHICopyBuffer(src, srcOffset, dst, dstOffset, size);
		}
	}

	void RHICommandList::CopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHICopyTexture(src, srcSubresource, dst, dstSubresource);
		}
	}

	void RHICommandList::GenerateMips(std::shared_ptr<RHITexture> src)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIGenerateMips(src);
		}
	}

	void RHICommandList::SetViewport(Offset2D min, Offset2D max)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHISetViewport(min, max);
		}
	}

	void RHICommandList::SetScissor(Offset2D min, Offset2D max)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHISetScissor(min, max);
		}
	}

	void RHICommandList::ClearScissors(const std::vector<ClearAttachment>& attachments, const std::vector<Rect2D>& scissors, uint32_t baseArrayLayer, uint32_t layerCount)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIClearScissors(attachments, scissors, baseArrayLayer, layerCount);
		}
	}

	void RHICommandList::SetDepthBias(float constantBias, float slopeBias, float clampBias)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHISetDepthBias(constantBias, slopeBias, clampBias);
		}
	}

	void RHICommandList::SetLineWidth(float width)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHISetLineWidth(width);
		}
	}

	void RHICommandList::SetGraphicsPipeline(std::shared_ptr<RHIGraphicsPipeline> graphicsPipeline)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHISetGraphicsPipeline(graphicsPipeline);
		}
	}

	void RHICommandList::BeginRendering(const RHIRenderPassInfo& info)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIBeginRendering(info);
		}
	}
	

	void RHICommandList::EndRendering()
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIEndRendering();
		}
	}

	void RHICommandList::PushConstants(void* data, uint16_t size, ShaderFrequency frequency)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIPushConstants(data, size, frequency);
		}
	}

	void RHICommandList::BindDescriptorSet(std::shared_ptr<RHIDescriptorSet> descriptor, uint32_t set)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIBindDescriptorSet(descriptor, set);
		}
	}

	void RHICommandList::BindVertexBuffer(std::shared_ptr<RHIBuffer> vertexBuffer, uint32_t streamIndex, uint32_t offset)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIBindVertexBuffer(vertexBuffer, streamIndex, offset);
		}
	}

	void RHICommandList::BindIndexBuffer(std::shared_ptr<RHIBuffer> indexBuffer, uint32_t offset)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIBindIndexBuffer(indexBuffer, offset);
		}
	}

	void RHICommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIDraw(vertexCount, instanceCount, firstVertex, firstInstance);
		}
	}

	void RHICommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
	{
		if (m_bypass)
		{
			assert(m_cmdContext);
			m_cmdContext->RHIDrawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}
	}


}