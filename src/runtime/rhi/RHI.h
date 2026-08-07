#pragma once

#include "RHIDefinitions.h"

#include <iostream>
#include <memory>
#include <glm/glm.hpp>

class SDL_Window;

// windows header contradiction
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

namespace shzk
{
	class RHIQueue;
	class RHISurface;
	class RHISwapchain;
	class RHICommandPool;
	class RHISemaphore;
	class RHIFence;
	class RHICommandContextImmediate;
	class RHITexture;
	class RHIGraphicsPipeline;

	class RHI
	{
	private:
		static std::shared_ptr<RHI> g_rhi;

	public:
		static std::shared_ptr<RHI> Init(const RHIInfo& rhiInfo);
		static std::shared_ptr<RHI>& Get() { return g_rhi; }

		virtual void Destroy() = 0;

		// Fundamentals
		virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) = 0;
		virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) = 0;
		virtual std::shared_ptr<RHISwapchain> CreateSwapchain(const RHISwapchainInfo& info) = 0;
		virtual std::shared_ptr<RHICommandPool> CreateCommandPool(const RHICommandPoolInfo& info) = 0;
		virtual std::shared_ptr<RHISemaphore> CreateSemaphore() = 0;
		virtual std::shared_ptr<RHIFence> CreateFence() = 0;

		// Resources
		virtual std::shared_ptr<RHIBuffer> CreateBuffer(const RHIBufferInfo& info) = 0;
		virtual std::shared_ptr<RHITexture> CreateTexture(const RHITextureInfo& info) = 0;
		virtual std::shared_ptr<RHITextureView> CreateTextureView(const RHITextureViewInfo& info) = 0;
		virtual std::shared_ptr<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineInfo& info) = 0;

		std::shared_ptr<RHICommandContextImmediate> GetCommandContextImmediate() const { return m_cmdContextImmediate; }

	protected:
		RHI() = delete;
		RHI(const RHIInfo& rhiInfo) : m_rhiInfo(rhiInfo) {}
		~RHI() = default;

	protected:
		RHIInfo m_rhiInfo;

		std::shared_ptr<RHICommandContextImmediate> m_cmdContextImmediate;
	};

	class RHIQueue
	{
	public:
		RHIQueue() = delete;
		RHIQueue(const RHIQueueInfo& info) : m_info(info) {}
		~RHIQueue() = default;

		inline RHIQueueType GetType() const { return m_info.type; }
		inline uint32_t GetQueueFamilyIndex() const { return m_info.index; }

		virtual void WaitIdle() = 0;

	protected:
		RHIQueueInfo m_info;
	};

	class RHISurface
	{
	public:
		RHISurface() = default;
		~RHISurface() = default;

		virtual void Destroy() = 0;

		inline Extent2D GetExetent() const { return m_extent; }

	protected:
		Extent2D m_extent;
	};

	class RHISwapchain
	{
	public:
		RHISwapchain() = delete;
		RHISwapchain(const RHISwapchainInfo& info) : m_info(info) {}
		~RHISwapchain() = default;

		virtual void Destroy() = 0;

		virtual uint32_t GetCurrentFrameIndex() = 0;
		virtual std::shared_ptr<RHITexture> GetTexture(uint32_t index) = 0;
		virtual std::shared_ptr<RHITexture> AcquireNextTexture(std::shared_ptr<RHIFence> fence, std::shared_ptr<RHISemaphore> signalSemaphore) = 0;
		virtual void Present(std::shared_ptr<RHISemaphore> waitSemaphore) = 0;

	protected:
		RHISwapchainInfo m_info;
	};

	class RHISemaphore
	{
	public:
		RHISemaphore() = default;
		~RHISemaphore() = default;

		virtual void Destroy() = 0;
	};

	class RHIFence
	{
	public:
		RHIFence() = default;
		~RHIFence() = default;

		virtual void Destroy() = 0;
		virtual void Wait() = 0;
	};

	class RHICommandContext;
	class RHICommandPool : public std::enable_shared_from_this<RHICommandPool>
	{
	public:
		RHICommandPool(const RHICommandPoolInfo& info)
			: m_info(info) {}
		~RHICommandPool() = default;

		virtual void Destroy() = 0;

		virtual std::shared_ptr<RHICommandContext> CreateCommandContext() = 0;

		inline std::shared_ptr<RHIQueue> GetQueue() { return m_info.queue; }

	private:
		RHICommandPoolInfo m_info;
	};

	class RHICommandContext
	{
	public:
		RHICommandContext() = default;
		~RHICommandContext() = default;

		virtual void Destroy() = 0;
		
		virtual void RHIBeginCommand() = 0;
		virtual void RHIEndCommand() = 0;
		virtual void RHISubmit(
			std::shared_ptr<RHIFence> fence,
			std::shared_ptr<RHISemaphore> waitSemaphore,
			std::shared_ptr<RHISemaphore> signalSemaphore) = 0;
		virtual void RHITextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba) = 0;
		virtual void RHITextureBarrierCommand(const RHITextureBarrier& barrier) = 0;
		/*
		virtual void Execute(RHIFenceRef waitFence, RHISemaphoreRef waitSemaphore, RHISemaphoreRef signalSemaphore) = 0;     // ʵ���ύ������ӳ�¼��Ҳ���ڶ�Ӧ�̵߳��øú������¼���ύ

		virtual void TextureBarrier(const RHITextureBarrier& barrier) = 0;

		virtual void BufferBarrier(const RHIBufferBarrier& barrier) = 0;

		virtual void CopyTextureToBuffer(RHITextureRef src, TextureSubresourceLayers srcSubresource, RHIBufferRef dst, uint64_t dstOffset) = 0;

		virtual void CopyBufferToTexture(RHIBufferRef src, uint64_t srcOffset, RHITextureRef dst, TextureSubresourceLayers dstSubresource) = 0;

		virtual void CopyBuffer(RHIBufferRef src, uint64_t srcOffset, RHIBufferRef dst, uint64_t dstOffset, uint64_t size) = 0;

		virtual void CopyTexture(RHITextureRef src, TextureSubresourceLayers srcSubresource, RHITextureRef dst, TextureSubresourceLayers dstSubresource) = 0;

		virtual void GenerateMips(RHITextureRef src) = 0;

		virtual void PushEvent(const std::string& name, Color3 color) = 0;   //Label?

		virtual void PopEvent() = 0;

		virtual void BeginRenderPass(RHIRenderPassRef renderPass) = 0;   //Ҳ��������ʱFindOrCreate��Ӧ��renderpass��framebuffer�ȣ��ܶණ�����������Ļ��Ĳ��ұ�ͳһ����״̬

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
	};

	class RHICommandContextImmediate
	{
	public:
		RHICommandContextImmediate() = default;
		~RHICommandContextImmediate() = default;

		virtual void Destroy() = 0;
		
		virtual void Flush() = 0;
		virtual void TextureBarrier(const RHITextureBarrier& barrier) = 0;
		virtual void BufferBarrier(const RHIBufferBarrier& barrier) = 0;
		virtual void CopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset) = 0;
		virtual void CopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) = 0;

		virtual void CopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size) = 0;

		virtual void CopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) = 0;

		virtual void GenerateMips(std::shared_ptr<RHITexture> src) = 0;
		
	};
}
