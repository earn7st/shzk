#include "VulkanRHI.h"
#include "VulkanUtil.h"
#include "VulkanRHISurface.h"

#define  VOLK_IMPLEMENTATION
#include <volk/volk.h>
#include <VkBootstrap.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace shzk
{
	VulkanRHI::VulkanRHI(const RHIInfo& rhiInfo)
		: RHI(rhiInfo)
	{
		CreateInstance();
		CreatePhysicalDevice();
		CreateLogicalDevice();
		CreateQueues();
		CreateMemoryAllocator();
		CreateDescriptorPool();
		CreateImmediateCommand();
	}

	std::shared_ptr<RHISurface> VulkanRHI::CreateSurface(SDL_Window* window)
	{
		std::shared_ptr<RHISurface> surface = std::make_shared<VulkanRHISurface>(window, *this);
		assert(surface);
		SHZK_LOG_INFO("Vulkan surface created.");
		return surface;
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

	void VulkanRHI::CreateLogicalDevice()
	{
		vkb::DeviceBuilder builder(m_vkbPhysicalDevice);

		auto result = builder.build();

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
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
		m_queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

		for (auto& index : m_queueIndices) index = -1;

		for (auto& queueFamilyProperty : m_queueFamilyProperties)
		{
			SHZK_LOG_INFO("Queue count: {}", queueFamilyProperty.queueCount);
			SHZK_LOG_INFO("Queue flags: {}", queueFamilyProperty.queueFlags);
		}

		std::vector<uint32_t> allocatedCounts(m_queueFamilyProperties.size());
		for (int i = 0; i < m_queueFamilyProperties.size(); i++)
		{
			auto& queueFamily = queueFamilyProperties[i];
			uint32_t queueCount = queueFamily.queueCount;   // 多个不同属性的队列可以从同一个队列族分配，只要数量够就行，队列族不只支持一种属性
			// 背包问题 XD
			if (queueCount > MAX_QUEUE_CNT &&
				queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT &&
				m_queueIndices[QUEUE_TYPE_GRAPHICS] < 0)
			{
				queueIndices[QUEUE_TYPE_GRAPHICS] = i;
				queueCount -= MAX_QUEUE_CNT;
			}
			if (queueCount > MAX_QUEUE_CNT &&
				queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT &&
				queueIndices[QUEUE_TYPE_COMPUTE] < 0)
			{
				queueIndices[QUEUE_TYPE_COMPUTE] = i;
				queueCount -= MAX_QUEUE_CNT;
			}
			if (queueCount > MAX_QUEUE_CNT &&
				queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT &&
				queueIndices[QUEUE_TYPE_TRANSFER] < 0)
			{
				queueIndices[QUEUE_TYPE_TRANSFER] = i;
				queueCount -= MAX_QUEUE_CNT;
			}

			allocatedCounts[i] = queueFamily.queueCount - queueCount;

			// 好像graphics queue就支持了？不需要单独处理？
			// // 窗口支持
			// VkBool32 presentSupport = false;
			// vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
			// if (queueFamily.queueCount > 0 && presentSupport && presentFamily < 0 && graphicsFamily != i) { //强行使用不同的queue
			//     presentFamily = i;
			// }
		}
		for (int i = 0; i < QUEUE_TYPE_MAX_ENUM; i++) if (queueIndices[i] < 0) LOG_FATAL("Fail to allocate queue!");

		std::vector<uint32_t> offsets{};
		offsets.resize(m_queueFamilyProperties.size());
		(m_queueFamilyProperties.size(), { 0 });
		for (uint32_t i = 0; i < QUEUE_TYPE_MAX_ENUM; i++)
		{
			for (uint32_t j = 0; j < MAX_QUEUE_CNT; j++)
			{
				VkQueue queue;
				vkGetDeviceQueue(logicalDevice, queueIndices[i], offsets[i], &queue);

				RHIQueueInfo info =
				{
					.type = (QueueType)i,
					.index = j,
				};
				queues[i][j] = std::make_shared<VulkanRHIQueue>(info, queue, queueIndices[i]);
				RegisterResource(queues[i][j]);

				offsets[i]++;
			}
		}
	}

	void VulkanRHI::CreateDescriptorPool()
	{

	}

	void VulkanRHI::CreateImmediateCommand()
	{

	}
}