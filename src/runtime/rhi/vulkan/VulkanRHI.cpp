#include "VulkanRHI.h"
#include "VulkanUtils.h"

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

	}

	void VulkanRHI::CreateDescriptorPool()
	{

	}

	void VulkanRHI::CreateImmediateCommand()
	{

	}
}