#pragma once

#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <vector>
#include <string>
#include <array>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

// windows header contradiction
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

namespace shzk
{
	class VulkanRHI : public RHI
	{
	public:
		VulkanRHI() = delete;
		VulkanRHI(const RHIInfo& rhiInfo);
		~VulkanRHI() = default;

		virtual void Destroy() override final;
		
		// Fundamentals
		virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) override final;
		virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) override final;
		virtual std::shared_ptr<RHISwapchain> CreateSwapchain(const RHISwapchainInfo& info) override final;
		virtual std::shared_ptr<RHICommandPool> CreateCommandPool(const RHICommandPoolInfo& info) override final;
		virtual std::shared_ptr<RHISemaphore> CreateSemaphore() override final;
		virtual std::shared_ptr<RHIFence> CreateFence() override final;

		// Resources
		virtual std::shared_ptr<RHIBuffer> CreateBuffer(const RHIBufferInfo& info) override final;
		virtual std::shared_ptr<RHITexture> CreateTexture(const RHITextureInfo& info) override final;
		virtual std::shared_ptr<RHITextureView> CreateTextureView(const RHITextureViewInfo& info) override final;
		virtual std::shared_ptr<RHIShader> CreateShader(const RHIShaderInfo& info) override final;
		virtual std::shared_ptr<RHIRootSignature> CreateRootSignature(const RHIRootSignatureInfo& info) override final;
		virtual std::shared_ptr<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineInfo& info) override final;
		virtual std::shared_ptr<RHISampler> CreateSampler(const RHISamplerInfo& info) override final;

		inline const VkInstance& GetInstance() { return m_instance; }
		inline const VkPhysicalDevice& GetPhysicalDevice() { return m_physicalDevice; }
		inline const VkDevice& GetDevice() { return m_device; }
		inline const VmaAllocator& GetAllocator() { return m_allocator; }
		inline const VkDescriptorPool& GetDescriptorPool() { return m_descriptorPool; }

		using RHI::RegisterResource;	// promote to public

	private:
		void CreateInstance();
		void CreatePhysicalDevice();
		void SelectQueueFamilies();
		void CreateLogicalDevice();
		void CreateMemoryAllocator();
		void CreateQueues();
		void CreateImmediateCommand();
		void CreateDescriptorPool();

	private:
		vkb::Instance m_vkbInstance;
		vkb::PhysicalDevice m_vkbPhysicalDevice;

		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		VkPhysicalDeviceMemoryProperties m_memoryProperties;
		std::vector<std::string> m_supportedExtensions;

		VkDevice m_device;

		VmaAllocator m_allocator;

		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
		std::array<int32_t, (size_t)RHIQueueType::Max> m_queueIndices;	// Family indices for a specific queue type
		std::array<int32_t, (size_t)RHIQueueType::Max> m_queueCounts;	// How many queues we have for a specific queue type
		std::array<std::array<std::shared_ptr<RHIQueue>, MAX_QUEUE_CNT>, (size_t)RHIQueueType::Max> m_queues;	// Containing RHIQueue type

		VkDescriptorPool m_descriptorPool;
	};

	class VulkanRHIGraphicsPipeline;

	class VulkanRHICommandContext : public RHICommandContext
	{
	public:
		VulkanRHICommandContext() = delete;
		VulkanRHICommandContext(VkCommandBuffer vkCmdBuffer, std::shared_ptr<RHICommandPool> cmdPool)
			: m_cmdBuffer(vkCmdBuffer), m_cmdPool(cmdPool) {
		}
		~VulkanRHICommandContext() = default;

		virtual void Destroy() override final;

		inline VkCommandBuffer& GetHandle() { return m_cmdBuffer; }
		inline VkPipelineLayout GetCurrentPipelineLayout();
		inline VkPipelineBindPoint GetCurrentPipelineBindPoint();

		// RHI Commands
		virtual void RHIBeginCommand() override final;
		virtual void RHIEndCommand() override final;
		virtual void RHISubmit(
			std::shared_ptr<RHIFence> waitFence,
			std::shared_ptr<RHISemaphore> waitSemaphore,
			std::shared_ptr<RHISemaphore> signalSemaphore) override final;

		virtual void RHITextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba) override final;
		virtual void RHITextureBarrierCommand(const RHITextureBarrier& barrier) override final;
		virtual void RHIBufferBarrierCommand(const RHIBufferBarrier& barrier) override final;
		virtual void RHICopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset) override final;
		virtual void RHICopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) override final;
		virtual void RHICopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size) override final;
		virtual void RHICopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) override final;
		virtual void RHIGenerateMips(std::shared_ptr<RHITexture> src) override final;

		virtual void RHISetViewport(Offset2D min, Offset2D max) override final;
		virtual void RHISetScissor(Offset2D min, Offset2D max) override final;
		virtual void RHIClearScissors(const std::vector<ClearAttachment>& attachments, const std::vector<Rect2D>& scissors, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1) override final;
		virtual void RHISetDepthBias(float constantBias, float slopeBias, float clampBias) override final;
		virtual void RHISetLineWidth(float width) override final;

		virtual void RHISetGraphicsPipeline(std::shared_ptr<RHIGraphicsPipeline> graphicsPipeline) override final;
		// virtual void RHISetComputePipeline(RHIComputePipelineRef computePipeline) override final;

		virtual void RHIBeginRendering() override final;
		virtual void RHIEndRendering() override final;

		virtual void RHIPushConstants(void* data, uint16_t size, ShaderFrequency frequency) override final;
		virtual void RHIBindDescriptorSet(std::shared_ptr<RHIDescriptorSet> descriptorSet, uint32_t set = 0) override final;
		virtual void RHIBindVertexBuffer(std::shared_ptr<RHIBuffer> vertexBuffer, uint32_t streamIndex = 0, uint32_t offset = 0) override final;
		virtual void RHIBindIndexBuffer(std::shared_ptr<RHIBuffer> indexBuffer, uint32_t offset = 0) override final;

		virtual void RHIDraw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override final;
		virtual void RHIDrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0) override final;

	private:
		std::shared_ptr<RHICommandPool> m_cmdPool = nullptr;
		VkCommandBuffer m_cmdBuffer;

		std::shared_ptr<VulkanRHIGraphicsPipeline> m_currentGraphicsPipeline = nullptr;
	};

	class VulkanRHICommandContextImmediate : public RHICommandContextImmediate
	{
	public:
		VulkanRHICommandContextImmediate() = delete;
		VulkanRHICommandContextImmediate(VulkanRHI& rhi);
		~VulkanRHICommandContextImmediate() = default;

		virtual void Destroy() override final;

		virtual void RHISubmit() override final;
		virtual void RHITextureBarrierCommand(const RHITextureBarrier& barrier) override final;
		virtual void RHIBufferBarrierCommand(const RHIBufferBarrier& barrier) override final;
		virtual void RHICopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset) override final;
		virtual void RHICopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) override final;
		virtual void RHICopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size) override final;
		virtual void RHICopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource) override final;
		virtual void RHIGenerateMips(std::shared_ptr<RHITexture> src) override final;

	private:
		void BeginImmediateCommand();
		void EndImmediateCommand();

		std::shared_ptr<RHIFence> m_fence;
		std::shared_ptr<RHIQueue> m_queue;
		std::shared_ptr<RHICommandPool> m_cmdPool;

		VkCommandBuffer m_handle	= VK_NULL_HANDLE;
		VkCommandBuffer m_oldHandle = VK_NULL_HANDLE;
		VkDevice m_device;
	};

	// implementations shared by command context and immediate command context
	void RHITextureBarrierImpl(VkCommandBuffer& cmdBuffer, const RHITextureBarrier& barrier);	
	void RHIBufferBarrierImpl(VkCommandBuffer& cmdBuffer, const RHIBufferBarrier& barrier);
	void RHICopyTextureToBufferImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset);
	void RHICopyBufferToTextureImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource);
	void RHICopyBufferImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size);
	void RHICopyTextureImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource);
	void RHIGenerateMipsImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHITexture> src);

	void RHIBlitTexture(VkCommandBuffer& cmdBuffer, 
		std::shared_ptr<RHITexture> src, std::shared_ptr<RHITexture> dst,
		TextureSubresourceLayers srcSubresource, TextureSubresourceLayers dstSubresource,
		FilterType filter);
}