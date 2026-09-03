#pragma once

#include "RHIDefinitions.h"

#include <memory>
#include <glm/glm.hpp>
#include <array>
#include <vector>

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
	class RHIComputePipeline;
	class RHIResource;
	class RHIRootSignature;
	class RHISampler;

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
		virtual std::shared_ptr<RHIShader>	CreateShader(const RHIShaderInfo& info) = 0;
		virtual std::shared_ptr<RHIRootSignature> CreateRootSignature(const RHIRootSignatureInfo& info) = 0;
		virtual std::shared_ptr<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineInfo& info) = 0;
		virtual std::shared_ptr<RHIComputePipeline> CreateComputePipeline(const RHIComputePipelineInfo& info) = 0;
		virtual std::shared_ptr<RHISampler> CreateSampler(const RHISamplerInfo& info) = 0;

		std::shared_ptr<RHICommandContextImmediate> GetCommandContextImmediate() const { return m_cmdContextImmediate; }	// TODO: RHICommandListImmediate

		void Tick();

	protected:
		RHI() = delete;
		RHI(const RHIInfo& rhiInfo) : m_rhiInfo(rhiInfo) {}
		~RHI() = default;

		void RegisterResource(std::shared_ptr<RHIResource> res);
		void DeferredResourceDeletes();	// should be called every tick, handle RHIResource deletion
		void DestroyAllResources();

	protected:
		RHIInfo m_rhiInfo;
		std::shared_ptr<RHICommandContextImmediate> m_cmdContextImmediate;
		// Resource Map
		std::array<std::vector<std::shared_ptr<RHIResource>>, (size_t)RHIResourceType::Max> m_resourceMap;
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

		virtual void RHIBlitTexture(std::shared_ptr<RHITexture> src, std::shared_ptr<RHITexture> dst,
			TextureSubresourceLayers srcSubresource, TextureSubresourceLayers dstSubresource,
			FilterType filter) = 0;
		virtual void RHITextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba) = 0;
		virtual void RHITextureBarrierCommand(const RHITextureBarrier& barrier) = 0;	// Add "Command" because conflict with struct "RHITextureBarrier"
		virtual void RHIBufferBarrierCommand(const RHIBufferBarrier& barrier) = 0;
		virtual void RHICopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset) = 0;
		virtual void RHICopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) = 0;
		virtual void RHICopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size) = 0;
		virtual void RHICopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) = 0;
		virtual void RHIGenerateMips(std::shared_ptr<RHITexture> src) = 0;

		virtual void RHISetViewport(Offset2D min, Offset2D max) = 0;
		virtual void RHISetScissor(Offset2D min, Offset2D max) = 0;
		virtual void RHIClearScissors(const std::vector<ClearAttachment>& attachments, const std::vector<Rect2D>& scissors, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1) = 0;
		virtual void RHISetDepthBias(float constantBias, float slopeBias, float clampBias) = 0;
		virtual void RHISetLineWidth(float width) = 0;

		virtual void RHISetGraphicsPipeline(std::shared_ptr<RHIGraphicsPipeline> graphicsPipeline) = 0;
		virtual void RHISetComputePipeline(std::shared_ptr<RHIComputePipeline> computePipeline) = 0;

		virtual void RHIBeginRendering(const RHIRenderPassInfo& info) = 0;
		virtual void RHIEndRendering() = 0;

		virtual void RHIPushConstants(void* data, uint16_t size, ShaderFrequency frequency) = 0;
		virtual void RHIBindDescriptorSet(std::shared_ptr<RHIDescriptorSet> descriptor, uint32_t set = 0) = 0;
		virtual void RHIBindVertexBuffer(std::shared_ptr<RHIBuffer> vertexBuffer, uint32_t streamIndex = 0, uint32_t offset = 0) = 0;
		virtual void RHIBindIndexBuffer(std::shared_ptr<RHIBuffer> indexBuffer, uint32_t offset = 0) = 0;

		virtual void RHIDispatch(uint32_t grouptCountX = 1, uint32_t groupCountY = 1, uint32_t groupCountZ = 1) = 0;

		virtual void RHIDraw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;
		virtual void RHIDrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0) = 0;
	};

	class RHICommandContextImmediate
	{
	public:
		RHICommandContextImmediate() = default;
		~RHICommandContextImmediate() = default;

		virtual void Destroy() = 0;
		
		virtual void RHISubmit() = 0;
		virtual void RHITextureBarrierCommand(const RHITextureBarrier& barrier) = 0;
		virtual void RHIBufferBarrierCommand(const RHIBufferBarrier& barrier) = 0;
		virtual void RHICopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset) = 0;
		virtual void RHICopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) = 0;
		virtual void RHICopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size) = 0;
		virtual void RHICopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) = 0;
		virtual void RHIGenerateMips(std::shared_ptr<RHITexture> src) = 0;
		
	};
}
