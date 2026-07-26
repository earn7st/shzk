#include "VulkanRHI.h"
#include "VulkanUtil.h"
#include "VulkanRHISurface.h"
#include "VulkanRHIQueue.h"
#include "VulkanRHICommandContext.h"
#include "VulkanRHICommandPool.h"
#include "VulkanRHISemaphore.h"
#include "VulkanRHIFence.h"
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
		
		//CreateDescriptorPool();
		//CreateImmediateCommand();
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

		// Graphics: 第一个带 GRAPHICS_BIT 的
		for (uint32_t i = 0; i < count; i++) {
			if (m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				m_queueIndices[(int)RHIQueueType::Graphics] = i;
				break;
			}
		}

		// Compute: 优先 Compute 专用 family（无 GRAPHICS 的 COMPUTE family）
		for (uint32_t i = 0; i < count; i++) {
			auto f = m_queueFamilyProperties[i].queueFlags;
			if ((f & VK_QUEUE_COMPUTE_BIT) && !(f & VK_QUEUE_GRAPHICS_BIT)) {
				m_queueIndices[(int)RHIQueueType::Compute] = i;
				break;
			}
		}
		// 若无专用 Compute，那么和 Graphics 共享
		if (m_queueIndices[(int)RHIQueueType::Compute] < 0)
			m_queueIndices[(int)RHIQueueType::Compute] = m_queueIndices[(int)RHIQueueType::Graphics];

		// Transfer: 优先纯 DMA family（无 GRAPHICS 且无 COMPUTE）
		for (uint32_t i = 0; i < count; i++) {
			auto f = m_queueFamilyProperties[i].queueFlags;
			if ((f & VK_QUEUE_TRANSFER_BIT) && !(f & VK_QUEUE_GRAPHICS_BIT) && !(f & VK_QUEUE_COMPUTE_BIT)) {
				m_queueIndices[(int)RHIQueueType::Transfer] = i;
				break;
			}
		}
		// 若无专用 Transfer，那么和 Graphics 共享
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
		// 只用传 Instance 和 Device 的函数入口地址，其他的函数都已经被 volk 处理好
		VmaVulkanFunctions vulkanFunctions{};
		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo createInfo{};
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		createInfo.physicalDevice = m_physicalDevice;
		createInfo.device = m_device;
		createInfo.instance = m_instance;
		createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
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

	void VulkanRHI::CreateDescriptorPool()
	{

	}

	void VulkanRHI::CreateImmediateCommand()
	{

	}
	
}