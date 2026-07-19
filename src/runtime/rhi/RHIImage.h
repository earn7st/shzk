#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace vkR::rhi
{
	class Device;

	class Image
	{
	public:
		Image() = delete;
		Image(
			Device*				device,
			VkExtent3D			extent,
			VkFormat			format,
			VkImageUsageFlags	usage,
			uint32_t			mipLevles,
			VkSampleCountFlagBits	samples,
			const char*			name);
		~Image();

		void UploadData(const void* data, VkDeviceSize size);

		VkExtent3D	GetExtent() const { return m_extent; }
		VkFormat	GetFormat() const { return m_format; }
		VkImage		GetImage() const { return m_image; }
		VkImageView GetImageView() const { return m_imageView; }

	private:
		void CreateResource();
		void DestroyResource();

	private:
		Device*			m_rhiDevice = nullptr;

		VkExtent3D				m_extent{};
		VkFormat				m_format{};
		VkImageUsageFlags		m_usage = 0;
		uint32_t				m_mipLevels = 0;
		VkSampleCountFlagBits	m_samples{};
		std::string				m_name;

		VkImage			m_image			= VK_NULL_HANDLE;
		VkImageView		m_imageView		= VK_NULL_HANDLE;
		VmaAllocation	m_allocation	= nullptr;
	};
	
}