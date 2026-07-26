#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHICommandContext : public RHICommandContext
	{
	public:
		VulkanRHICommandContext() = delete;
		VulkanRHICommandContext(VkCommandBuffer vkCmdBuffer)
			: m_cmdBuffer(vkCmdBuffer) {
		}

		virtual void BeginCommand() override final;
		virtual void EndCommand() override final;

		inline VkCommandBuffer GetHandle() const { return m_cmdBuffer; }
		/*
		virtual void Execute(RHIFenceRef waitFence, RHISemaphoreRef waitSemaphore, RHISemaphoreRef signalSemaphore) = 0;     // 实际提交，如果延迟录制也该在对应线程调用该函数完成录制提交

		// UE RHI彻底做了资源状态（如VkImageLayout）等的屏蔽封装，代价是极其痛苦的RHI实现
		// 和BeginTransitions，FVulkanLayoutManager等有关
		// 参考Sakura Engine还是做暴露吧，与UE和解
		// resource state的屏蔽应该在RDG等层级处理，而不是RHI

		virtual void TextureBarrier(const RHITextureBarrier& barrier) = 0;

		virtual void BufferBarrier(const RHIBufferBarrier& barrier) = 0;

		virtual void CopyTextureToBuffer(RHITextureRef src, TextureSubresourceLayers srcSubresource, RHIBufferRef dst, uint64_t dstOffset) = 0;

		virtual void CopyBufferToTexture(RHIBufferRef src, uint64_t srcOffset, RHITextureRef dst, TextureSubresourceLayers dstSubresource) = 0;

		virtual void CopyBuffer(RHIBufferRef src, uint64_t srcOffset, RHIBufferRef dst, uint64_t dstOffset, uint64_t size) = 0;

		virtual void CopyTexture(RHITextureRef src, TextureSubresourceLayers srcSubresource, RHITextureRef dst, TextureSubresourceLayers dstSubresource) = 0;

		virtual void GenerateMips(RHITextureRef src) = 0;

		virtual void PushEvent(const std::string& name, Color3 color) = 0;   //Label?

		virtual void PopEvent() = 0;

		virtual void BeginRenderPass(RHIRenderPassRef renderPass) = 0;   //也可以运行时FindOrCreate相应的renderpass和framebuffer等，很多东西可以做中心化的查找表统一管理状态

		virtual void EndRenderPass() = 0;

		virtual void SetViewport(Offset2D min, Offset2D max) = 0;

		virtual void SetScissor(Offset2D min, Offset2D max) = 0;

		virtual void ClearScissors(const std::vector<ClearAttachment>& attachments, const std::vector<Rect2D>& scissors, uint32_t baseArrayLayer, uint32_t layerCount) = 0;

		virtual void SetDepthBias(float constantBias, float slopeBias, float clampBias) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual void SetGraphicsPipeline(RHIGraphicsPipelineRef graphicsPipeline) = 0;

		virtual void SetComputePipeline(RHIComputePipelineRef computePipeline) = 0;

		virtual void SetRayTracingPipeline(RHIRayTracingPipelineRef rayTracingPipeline) = 0;

		virtual void PushConstants(void* data, uint16_t size, ShaderFrequency frequency) = 0;

		virtual void BindDescriptorSet(RHIDescriptorSetRef descriptor, uint32_t set) = 0;

		virtual void BindVertexBuffer(RHIBufferRef vertexBuffer, uint32_t streamIndex, uint32_t offset) = 0;

		virtual void BindIndexBuffer(RHIBufferRef indexBuffer, uint32_t offset) = 0;

		virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

		virtual void DispatchIndirect(RHIBufferRef argumentBuffer, uint32_t argumentOffset) = 0;

		virtual void TraceRays(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

		virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

		virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;

		virtual void DrawIndirect(RHIBufferRef argumentBuffer, uint32_t offset, uint32_t drawCount) = 0;

		virtual void DrawIndexedIndirect(RHIBufferRef argumentBuffer, uint32_t offset, uint32_t drawCount) = 0;

		// TODO
		// virtual void BeginRenderQuery(RHIRenderQuery* RenderQuery) = 0;

		// virtual void EndRenderQuery(RHIRenderQuery* RenderQuery) = 0;

		//ImGui /////////////////////////////////////////////////////////////////////////////////////

		virtual void ImGuiCreateFontsTexture() = 0;

		virtual void ImGuiRenderDrawData(ImGuiDrawFunc func) = 0;
		*/

	private:
		VkCommandBuffer m_cmdBuffer;

	};
}