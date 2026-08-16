#include "VulkanRHI.h"
#include "VulkanUtil.h"
#include "VulkanRHISurface.h"
#include "VulkanRHIQueue.h"
#include "VulkanRHISwapchain.h"
#include "VulkanRHICommandPool.h"
#include "VulkanRHISemaphore.h"
#include "VulkanRHIFence.h"
#include "VulkanRHIResource.h"
#include "runtime/rhi/RHIDefinitions.h"
#include "runtime/rhi/RHIResource.h"

#define  VOLK_IMPLEMENTATION
#include <volk/volk.h>
#include <VkBootstrap.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <vector>
#include <unordered_map>

namespace shzk
{
	VulkanRHI::VulkanRHI(const RHIInfo& rhiInfo)
		: RHI(rhiInfo)
	{
		CreateInstance();
		CreatePhysicalDevice();
		SelectQueueFamilies();
		CreateLogicalDevice();
		CreateQueues();
		CreateMemoryAllocator();
		CreateImmediateCommand();
		CreateDescriptorPool();
	}

	void VulkanRHI::Destroy()
	{
		SHZK_LOG_INFO("Vulkan RHI destroy begin");

		for (auto& queueList : m_queues)
		{
			for (auto& queue : queueList)
			{
				queue->WaitIdle();
			}
		}

		DestroyAllResources();

		// Reverse order of construction
		// Descriptor pool
		if (m_descriptorPool)
		{
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
		}

		// Command context immediate
		if (m_cmdContextImmediate)
		{
			m_cmdContextImmediate->Destroy();
		}

		// VMA allocator
		if (m_allocator)
		{
			vmaDestroyAllocator(m_allocator);
			m_allocator = VK_NULL_HANDLE;
		}

		// Logical device
		if (m_device)
		{
			vkDestroyDevice(m_device, nullptr);
			m_device = VK_NULL_HANDLE;
		}

		// Debug messenger (before instance)
		if (m_debugMessenger)
		{
			vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
			m_debugMessenger = VK_NULL_HANDLE;
		}

		// Instance
		if (m_instance)
		{
			vkDestroyInstance(m_instance, nullptr);
			m_instance = VK_NULL_HANDLE;
		}

		SHZK_LOG_INFO("Vulkan RHI destroy complete");
	}

	std::shared_ptr<RHIQueue> VulkanRHI::GetQueue(const RHIQueueInfo& info)
	{
		return m_queues[(size_t)info.type][(size_t)info.index];
	}

	std::shared_ptr<RHISurface> VulkanRHI::CreateSurface(SDL_Window* window)
	{
		std::shared_ptr<RHISurface> surface = std::make_shared<VulkanRHISurface>(window, *this);
		assert(surface);
		SHZK_LOG_INFO("Vulkan surface created");
		return surface;
	}

	std::shared_ptr<RHISwapchain> VulkanRHI::CreateSwapchain(const RHISwapchainInfo& info)
	{
		std::shared_ptr<RHISwapchain> swapchain = std::make_shared<VulkanRHISwapchain>(info, *this);
		assert(swapchain);
		SHZK_LOG_INFO("Vulkan swapchain created");
		return swapchain;
	}

	std::shared_ptr<RHICommandPool> VulkanRHI::CreateCommandPool(const RHICommandPoolInfo& info)
	{
		std::shared_ptr<RHICommandPool> cmdPool = std::make_shared<VulkanRHICommandPool>(info, *this);
		assert(cmdPool);
		SHZK_LOG_INFO("Vulkan command pool of queue type {} created", static_cast<int>(info.queue->GetType()));
		return cmdPool;
	}
	std::shared_ptr<RHISemaphore> VulkanRHI::CreateSemaphore()
	{
		std::shared_ptr<RHISemaphore> semaphore = std::make_shared<VulkanRHISemaphore>(*this);
		assert(semaphore);
		SHZK_LOG_INFO("Vulkan semaphore created");
		return semaphore;
	}

	std::shared_ptr<RHIFence> VulkanRHI::CreateFence()
	{
		std::shared_ptr<RHIFence> fence = std::make_shared<VulkanRHIFence>(*this);
		assert(fence);
		SHZK_LOG_INFO("Vulkan fence created");
		return fence;
	}

	std::shared_ptr<RHIBuffer> VulkanRHI::CreateBuffer(const RHIBufferInfo& info)
	{
		std::shared_ptr<RHIBuffer> buffer = std::make_shared<VulkanRHIBuffer>(info, *this);
		assert(buffer);
		RegisterResource(buffer);
		return buffer;
	}

	std::shared_ptr<RHITexture> VulkanRHI::CreateTexture(const RHITextureInfo& info)
	{
		std::shared_ptr<RHITexture> texture = std::make_shared<VulkanRHITexture>(info, *this, nullptr);
		assert(texture);
		RegisterResource(texture);
		return texture;
	}

	std::shared_ptr<RHITextureView> VulkanRHI::CreateTextureView(const RHITextureViewInfo& info)
	{
		std::shared_ptr<RHITextureView> view = std::make_shared<VulkanRHITextureView>(info, *this);
		assert(view);
		RegisterResource(view);
		return view;
	}

	std::shared_ptr<RHIGraphicsPipeline> VulkanRHI::CreateGraphicsPipeline(const RHIGraphicsPipelineInfo& info)
	{
		std::shared_ptr<RHIGraphicsPipeline> pipeline = std::make_shared<VulkanRHIGraphicsPipeline>(info, *this);
		assert(pipeline);
		RegisterResource(pipeline);
		return pipeline;
	}

	std::shared_ptr<RHIShader> VulkanRHI::CreateShader(const RHIShaderInfo& info)
	{
		std::shared_ptr<RHIShader> shader = std::make_shared<VulkanRHIShader>(info, *this);
		RegisterResource(shader);

		return shader;
	}

	void VulkanRHI::CreateInstance()
	{
		VK_CHECK(volkInitialize());

		vkb::InstanceBuilder builder;
		auto result = builder
			.set_app_name("SHZK")
			.set_engine_name("SHZK Engine")
			.require_api_version(1, 3)
			.use_default_debug_messenger()
			.enable_validation_layers(m_rhiInfo.debug)
			.build(); 

		if (!result) {
			SHZK_LOG_ERROR("Failed to create Vulkan instance: {}",
				result.error().message());
			assert(false);
			return;
		}

		m_vkbInstance = result.value();
		m_instance = m_vkbInstance.instance;
		m_debugMessenger = m_vkbInstance.debug_messenger;

		volkLoadInstance(m_instance);

		SHZK_LOG_INFO("Vulkan instance created (apiVersion: {}.{}.{})",
			VK_API_VERSION_MAJOR(m_vkbInstance.api_version),
			VK_API_VERSION_MINOR(m_vkbInstance.api_version),
			VK_API_VERSION_PATCH(m_vkbInstance.api_version));
	}

	void VulkanRHI::CreatePhysicalDevice()
	{
		vkb::PhysicalDeviceSelector selector(m_vkbInstance);

		auto result = selector
			.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
			.allow_any_gpu_device_type(true)
			.defer_surface_initialization()
			.set_minimum_version(1, 3)
			.select();

		if (!result) {
			SHZK_LOG_ERROR("Failed to select physical device: {}",
				result.error().message());
			assert(false);
			return;
		}

		m_vkbPhysicalDevice = result.value();
		m_physicalDevice = m_vkbPhysicalDevice.physical_device;
		m_properties = m_vkbPhysicalDevice.properties;
		m_features = m_vkbPhysicalDevice.features;
		m_memoryProperties = m_vkbPhysicalDevice.memory_properties;
		m_supportedExtensions = m_vkbPhysicalDevice.get_available_extensions();

		/*
		for (auto extension : m_supportedExtensions)
		{
			SHZK_LOG_INFO("Supported extension: {}", extension.c_str());
		}
		*/

		SHZK_LOG_INFO("Physical device selected: {}", m_properties.deviceName);
	}

	void VulkanRHI::SelectQueueFamilies()
	{
		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, nullptr);
		m_queueFamilyProperties.resize(count);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, m_queueFamilyProperties.data());

		for (auto& idx : m_queueIndices) idx = -1;
		for (auto& c : m_queueCounts) c = 0;

		for (uint32_t i = 0; i < count; i++) {
			if (m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				m_queueIndices[(int)RHIQueueType::Graphics] = i;
				break;
			}
		}

		for (uint32_t i = 0; i < count; i++) {
			auto f = m_queueFamilyProperties[i].queueFlags;
			if ((f & VK_QUEUE_COMPUTE_BIT) && !(f & VK_QUEUE_GRAPHICS_BIT)) {
				m_queueIndices[(int)RHIQueueType::Compute] = i;
				break;
			}
		}

		if (m_queueIndices[(int)RHIQueueType::Compute] < 0)
			m_queueIndices[(int)RHIQueueType::Compute] = m_queueIndices[(int)RHIQueueType::Graphics];

		for (uint32_t i = 0; i < count; i++) {
			auto f = m_queueFamilyProperties[i].queueFlags;
			if ((f & VK_QUEUE_TRANSFER_BIT) && !(f & VK_QUEUE_GRAPHICS_BIT) && !(f & VK_QUEUE_COMPUTE_BIT)) {
				m_queueIndices[(int)RHIQueueType::Transfer] = i;
				break;
			}
		}

		if (m_queueIndices[(int)RHIQueueType::Transfer] < 0)
			m_queueIndices[(int)RHIQueueType::Transfer] = m_queueIndices[(int)RHIQueueType::Graphics];
	}

	void VulkanRHI::CreateLogicalDevice()
	{
		std::unordered_map<uint32_t, size_t> uniqueFamilies;
		for (int t = 0; t < (int)RHIQueueType::Max; ++t)
		{
			uint32_t idx = m_queueIndices[t];
			if (idx >= 0)
			{
				uniqueFamilies[idx] = (size_t)MAX_QUEUE_CNT;
			}
		}

		std::vector<vkb::CustomQueueDescription> queueDescs;
		for (auto& [familyIdx, count] : uniqueFamilies)
		{
			std::vector<float> priorities(count, 1.0f);
			queueDescs.push_back(vkb::CustomQueueDescription(familyIdx, priorities));
		}

		vkb::DeviceBuilder builder(m_vkbPhysicalDevice);
		auto result = builder
			.custom_queue_setup(queueDescs)
			.build();

		if (!result) {
			SHZK_LOG_ERROR("Failed to create logical device: {}",
				result.error().message());
			assert(false);
			return;
		}

		auto vkbDevice = result.value();
		m_device = vkbDevice.device;

		volkLoadDevice(m_device);

		SHZK_LOG_INFO("Logical device created successfully");
	}

	void VulkanRHI::CreateMemoryAllocator()
	{
		VmaVulkanFunctions vulkanFunctions{};
		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo createInfo{};
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		createInfo.physicalDevice = m_physicalDevice;
		createInfo.device = m_device;
		createInfo.instance = m_instance;
		createInfo.flags = 0;	// TODO VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT -> bindless
		createInfo.pVulkanFunctions = &vulkanFunctions;

		VK_CHECK(vmaCreateAllocator(&createInfo, &m_allocator));

		SHZK_LOG_INFO("VMA allocator created");
	}

	void VulkanRHI::CreateQueues()
	{	
		for (int t = 0; t < (int)RHIQueueType::Max; t++) {
			int32_t familyIdx = m_queueIndices[t];
			if (familyIdx < 0) {
				SHZK_LOG_ERROR("No queue family for type {}", t);
				continue;
			}

			uint32_t available = m_queueFamilyProperties[familyIdx].queueCount;
			uint32_t toAlloc = std::min((uint32_t)MAX_QUEUE_CNT, available);

			for (uint32_t j = 0; j < toAlloc; j++) {
				VkQueue vkQueue;
				vkGetDeviceQueue(m_device, familyIdx, j, &vkQueue);

				RHIQueueInfo info{ (RHIQueueType)t, j };
				m_queues[t][j] = std::make_shared<VulkanRHIQueue>(info, vkQueue, familyIdx);

				// SHZK_LOG_INFO("RHIQueue of type {} created", t);
			}
			m_queueCounts[t] = toAlloc;
		}
	}

	void VulkanRHI::CreateImmediateCommand()
	{
		m_cmdContextImmediate = std::make_shared<VulkanRHICommandContextImmediate>(*this);
	}

	void VulkanRHI::CreateDescriptorPool()
	{
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 4096 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4096 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4096 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4096 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 4096 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 4096 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4096 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4096 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 4096 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 4096 },
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		poolInfo.pPoolSizes = descriptorPoolSizes.data();
		poolInfo.maxSets = 8192;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;   //Enable Bind then Update, Bindless

		VK_CHECK(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool));
	}

// Command Context
	
	void VulkanRHICommandContext::Destroy()
	{
		vkFreeCommandBuffers(VULKAN_RHI()->GetDevice(), CastTo<VulkanRHICommandPool>(m_cmdPool)->GetHandle(), 1, &m_cmdBuffer);
		//SHZK_LOG_INFO("VulkanRHICommandContext destroyed");
	}

	void VulkanRHICommandContext::RHIBeginCommand()
	{
		vkResetCommandBuffer(m_cmdBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_CHECK(vkBeginCommandBuffer(m_cmdBuffer, &beginInfo));
	}

	void VulkanRHICommandContext::RHIEndCommand()
	{
		VK_CHECK(vkEndCommandBuffer(m_cmdBuffer));
	}

	void VulkanRHICommandContext::RHISubmit(std::shared_ptr<RHIFence> fence, std::shared_ptr<RHISemaphore> waitSemaphore, std::shared_ptr<RHISemaphore> signalSemaphore)
	{
		VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkFence signalFence = VK_NULL_HANDLE;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_cmdBuffer;

		if (fence != nullptr)
		{
			signalFence = CastTo<VulkanRHIFence>(fence)->GetHandle();
		}
		if (waitSemaphore != nullptr)
		{
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &(CastTo<VulkanRHISemaphore>(waitSemaphore)->GetHandle());
			submitInfo.pWaitDstStageMask = &stage;
		}
		if (signalSemaphore != nullptr)
		{
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &(CastTo<VulkanRHISemaphore>(signalSemaphore)->GetHandle());
		}

		VK_CHECK((vkQueueSubmit(CastTo<VulkanRHIQueue>(m_cmdPool->GetQueue())->GetHandle(), 1, &submitInfo, signalFence)));
	}

	void VulkanRHICommandContext::RHITextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba)
	{
		VkClearColorValue clear = { rgba.x, rgba.y, rgba.z, rgba.a };
		VkImageSubresourceRange range = VulkanUtil::SubresourceToVk(
			texture->GetDefaultSubresourceRange());

		vkCmdClearColorImage(m_cmdBuffer,
			CastTo<VulkanRHITexture>(texture)->GetHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			&clear, 1, &range);
	}

	void VulkanRHICommandContext::RHITextureBarrierCommand(const RHITextureBarrier& barrier)	
	{
		RHITextureBarrierImpl(m_cmdBuffer, barrier);
	}

// Immediate Command Context

	VulkanRHICommandContextImmediate::VulkanRHICommandContextImmediate(VulkanRHI& rhi)
	{
		m_fence = rhi.CreateFence();
		m_queue = rhi.GetQueue({ .type = RHIQueueType::Graphics, .index = 0 });
		m_cmdPool = rhi.CreateCommandPool({ .queue = m_queue });
		m_device = rhi.GetDevice();		// save device handle here
										// because immediate commands can be called frequently, better not be casting global RHI every frame

		BeginImmediateCommand();
		// VkCommandBuffer is instantly created when use
	} 

	void VulkanRHICommandContextImmediate::Destroy()
	{
		m_fence->Destroy();
		m_cmdPool->Destroy();
	}

	void VulkanRHICommandContextImmediate::RHISubmit()
	{
		EndImmediateCommand();
		BeginImmediateCommand();
	}

	void VulkanRHICommandContextImmediate::RHITextureBarrierCommand(const RHITextureBarrier& barrier)
	{
		RHITextureBarrierImpl(m_handle, barrier);
	}

	void VulkanRHICommandContextImmediate::RHIBufferBarrierCommand(const RHIBufferBarrier& barrier)
	{
		RHIBufferBarrierImpl(m_handle, barrier);
	}

	void VulkanRHICommandContextImmediate::RHICopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset)
	{
		RHICopyTextureToBufferImpl(m_handle, src, srcSubresource, dst, dstOffset);
	}

	void VulkanRHICommandContextImmediate::RHICopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource)
	{
		RHICopyBufferToTextureImpl(m_handle, src, srcOffset, dst, dstSubresource);
	}

	void VulkanRHICommandContextImmediate::RHICopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size)
	{
		RHICopyBufferImpl(m_handle, src, srcOffset, dst, dstOffset, size);
	}

	void VulkanRHICommandContextImmediate::RHICopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource)
	{
		RHICopyTextureImpl(m_handle, src, srcSubresource, dst, dstSubresource);
	}

	void VulkanRHICommandContextImmediate::RHIGenerateMips(std::shared_ptr<RHITexture> src)
	{
		RHIGenerateMipsImpl(m_handle, src);
	}

	void VulkanRHICommandContextImmediate::BeginImmediateCommand()
	{
		// reallocate VkCommandBuffer
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = CastTo<VulkanRHICommandPool>(m_cmdPool)->GetHandle();
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(m_device, &allocInfo, &m_handle);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(m_handle, &beginInfo);
	}

	void VulkanRHICommandContextImmediate::EndImmediateCommand()
	{
		m_fence->Wait(); 
		if (m_oldHandle != VK_NULL_HANDLE)
		{
			vkFreeCommandBuffers(VULKAN_RHI()->GetDevice(), CastTo<VulkanRHICommandPool>(m_cmdPool)->GetHandle(), 1, &m_oldHandle);
		}
		// free the old CommandBuffer when vkQueueSubmit finished

		VK_CHECK(vkEndCommandBuffer(m_handle));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_handle;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		VK_CHECK(vkQueueSubmit(CastTo<VulkanRHIQueue>(m_queue)->GetHandle(), 1, &submitInfo, CastTo<VulkanRHIFence>(m_fence)->GetHandle()));
		m_oldHandle = m_handle;
	}

	// shared implementations
	void RHITextureBarrierImpl(VkCommandBuffer& cmdBuffer, const RHITextureBarrier& barrier)
	{
		RHIResourceState srcState = barrier.srcState;
		RHIResourceState dstState = barrier.dstState;

		TextureSubresourceRange range = barrier.subresource;
		if (range.aspect == TEXTURE_ASPECT_NONE) range = barrier.texture->GetDefaultSubresourceRange();

		VkAccessFlags srcAccessMask = VulkanUtil::ResourceStateToVkAccessFlags(barrier.srcState);
		VkAccessFlags dstAccessMask = VulkanUtil::ResourceStateToVkAccessFlags(barrier.dstState);
		VkPipelineStageFlags srcStage = VulkanUtil::VkAccessFlagsToVkPipelineStageFlags(srcAccessMask);
		VkPipelineStageFlags dstStage = VulkanUtil::VkAccessFlagsToVkPipelineStageFlags(dstAccessMask);

		VkImageMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.oldLayout = VulkanUtil::ResourceStateToVkImageLayout(barrier.srcState);
		memoryBarrier.newLayout = VulkanUtil::ResourceStateToVkImageLayout(barrier.dstState);
		memoryBarrier.image = CastTo<VulkanRHITexture>(barrier.texture)->GetHandle();
		memoryBarrier.subresourceRange = VulkanUtil::SubresourceToVk(range);
		memoryBarrier.srcAccessMask = srcAccessMask;
		memoryBarrier.dstAccessMask = dstAccessMask;

		vkCmdPipelineBarrier(
			cmdBuffer,
			srcStage, dstStage, 0,
			0, nullptr,
			0, nullptr,
			1, &memoryBarrier);
	}

	void RHIBufferBarrierImpl(VkCommandBuffer& cmdBuffer, const RHIBufferBarrier& barrier)
	{
		VkAccessFlags srcAccessMask = VulkanUtil::ResourceStateToVkAccessFlags(barrier.srcState);
		VkAccessFlags dstAccessMask = VulkanUtil::ResourceStateToVkAccessFlags(barrier.dstState);
		VkPipelineStageFlags srcStage = VulkanUtil::VkAccessFlagsToVkPipelineStageFlags(srcAccessMask);
		VkPipelineStageFlags dstStage = VulkanUtil::VkAccessFlagsToVkPipelineStageFlags(dstAccessMask);

		VkBufferMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier.srcAccessMask = srcAccessMask;
		memoryBarrier.dstAccessMask = dstAccessMask;
		memoryBarrier.buffer = CastTo<VulkanRHIBuffer>(barrier.buffer)->GetHandle();
		memoryBarrier.offset = barrier.offset;
		memoryBarrier.size = barrier.size == 0 ? VK_WHOLE_SIZE : barrier.size;

		vkCmdPipelineBarrier(
			cmdBuffer,
			srcStage, dstStage, 0,
			0, nullptr,
			1, &memoryBarrier,
			0, nullptr);
	}

	void RHICopyTextureToBufferImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset)
	{
		VkBufferImageCopy copy = {};
		copy.bufferOffset = dstOffset;
		copy.bufferRowLength = 0;
		copy.bufferImageHeight = 0;
		copy.imageSubresource = VulkanUtil::SubresourceToVk(srcSubresource);
		copy.imageOffset = { 0, 0, 0 };
		copy.imageExtent = VulkanUtil::ExtentToVk(src->MipExtent(srcSubresource.mipLevel));

		vkCmdCopyImageToBuffer(cmdBuffer,
			CastTo<VulkanRHITexture>(src)->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			CastTo<VulkanRHIBuffer>(dst)->GetHandle(),
			1, &copy);
	}

	void RHICopyBufferToTextureImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource)
	{
		VkBufferImageCopy copy = {};
		copy.bufferOffset = srcOffset;
		copy.bufferRowLength = 0;
		copy.bufferImageHeight = 0;
		copy.imageSubresource = VulkanUtil::SubresourceToVk(dstSubresource);
		copy.imageOffset = { 0, 0, 0 };
		copy.imageExtent = VulkanUtil::ExtentToVk(dst->MipExtent(dstSubresource.mipLevel));

		vkCmdCopyBufferToImage(cmdBuffer,
			CastTo<VulkanRHIBuffer>(src)->GetHandle(),
			CastTo<VulkanRHITexture>(dst)->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &copy);
	}

	void RHICopyBufferImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size)
	{
		VkBufferCopy copy{};
		copy.srcOffset = srcOffset;
		copy.dstOffset = dstOffset;
		copy.size = size;

		vkCmdCopyBuffer(cmdBuffer,
			CastTo<VulkanRHIBuffer>(src)->GetHandle(),
			CastTo<VulkanRHIBuffer>(dst)->GetHandle(),
			1, &copy);
	}

	void RHICopyTextureImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource)
	{
		VkImageCopy imageCopy = {};
		imageCopy.srcOffset = { 0, 0, 0 };
		imageCopy.dstOffset = { 0, 0, 0 };
		imageCopy.srcSubresource = (srcSubresource.aspect == 0) ? VulkanUtil::SubresourceToVk(src->GetDefaultSubresourceLayers()) : VulkanUtil::SubresourceToVk(srcSubresource);
		imageCopy.dstSubresource = (dstSubresource.aspect == 0) ? VulkanUtil::SubresourceToVk(dst->GetDefaultSubresourceLayers()) : VulkanUtil::SubresourceToVk(dstSubresource);
		imageCopy.extent = VulkanUtil::ExtentToVk(src->MipExtent(srcSubresource.mipLevel));

		vkCmdCopyImage(cmdBuffer,
			CastTo<VulkanRHITexture>(src)->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			CastTo<VulkanRHITexture>(dst)->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &imageCopy);
	}

	void RHIGenerateMipsImpl(VkCommandBuffer& cmdBuffer, std::shared_ptr<RHITexture> src)
	{
		uint32_t mipLevels = src->GetInfo().mipLevels;
		if (mipLevels <= 1) return;

		uint32_t baseMipLevel	= 0;
		uint32_t baseArrayLayer = 0;

		for (uint32_t i = 0; i < src->GetInfo().arrayLayers; ++i)
		{
			baseMipLevel		= 0;
			baseArrayLayer	= i;
			
			RHITextureBarrier barrier{};
			barrier.texture	= src;
			barrier.srcState = RHIResourceState::TransferSrc;
			barrier.dstState = RHIResourceState::TransferDst;
			barrier.subresource = {
				TEXTURE_ASPECT_COLOR,
				1, mipLevels - 1,
				baseArrayLayer, 1};
			RHITextureBarrierImpl(cmdBuffer, barrier);

			for (uint32_t j = 0; j < mipLevels - 1; ++j)	// Generate one mip level at a time
			{
				TextureAspectFlags	  aspect = TEXTURE_ASPECT_NONE;
				uint32_t              mipLevel = 0;
				uint32_t              baseArrayLayer = 0;
				uint32_t              layerCount = 0;
				RHIBlitTexture(
					cmdBuffer,
					src, src,
					{TEXTURE_ASPECT_COLOR, baseMipLevel + j, baseArrayLayer, 1}, 
					{TEXTURE_ASPECT_COLOR, baseMipLevel + j + 1, baseArrayLayer, 1 },
					FilterType::Linear
				);

				RHITextureBarrier barrier{};
				barrier.texture = src;
				barrier.srcState = RHIResourceState::TransferDst;
				barrier.dstState = RHIResourceState::TransferSrc;
				barrier.subresource = {
					TEXTURE_ASPECT_COLOR,
					baseMipLevel + j + 1, 1,
					baseArrayLayer, 1 };
				RHITextureBarrierImpl(cmdBuffer, barrier);
			}
		}

	}
	void RHIBlitTexture(VkCommandBuffer& cmdBuffer, 
		std::shared_ptr<RHITexture> src, std::shared_ptr<RHITexture> dst, 
		TextureSubresourceLayers srcSubresource, TextureSubresourceLayers dstSubresource, 
		FilterType filter)
	{
		VkImageSubresourceLayers srcLayer = VulkanUtil::SubresourceToVk(srcSubresource);
		VkImageSubresourceLayers dstLayer = VulkanUtil::SubresourceToVk(dstSubresource);

		uint32_t srcMip = srcSubresource.mipLevel;
		uint32_t dstMip = dstSubresource.mipLevel;

		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { (int32_t)(src->GetInfo().extent.width / pow(2, srcMip)),
								(int32_t)(src->GetInfo().extent.height / pow(2, srcMip)), 1 };
		blit.srcSubresource = srcLayer;

		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { (int32_t)(dst->GetInfo().extent.width / pow(2, dstMip)),
								(int32_t)(dst->GetInfo().extent.height / pow(2, dstMip)), 1 };
		blit.dstSubresource = dstLayer;

		vkCmdBlitImage(cmdBuffer,
			CastTo<VulkanRHITexture>(src)->GetHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			CastTo<VulkanRHITexture>(dst)->GetHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&blit,
			VulkanUtil::FilterTypeToVk(filter));
	}
}