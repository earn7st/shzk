#include "RHIImage.h"
#include "RHIDevice.h"
#include "RHIUtil.h"

#include <vulkan/vulkan.h>

namespace vkR::rhi
{
	Image::Image(
		Device*				device,
		VkExtent3D			extent,
		VkFormat			format,
		VkImageUsageFlags	usage,
		uint32_t			mipLevels,
		VkSampleCountFlagBits	samples,
		const char* name)
	{
		m_rhiDevice = device;
		m_extent = extent;
		m_format = format;
		m_usage = usage;
		m_mipLevels = mipLevels;
		m_samples = samples;
		m_name = name;

		CreateResource();
	}

	Image::~Image()
	{
		DestroyResource();
	}

	void Image::UploadData(const void* data, VkDeviceSize size)
	{
		if (!data || size == 0) return;

		m_rhiDevice->UploadDataToImage(
			m_image,
			m_extent,
			m_usage,
			data,
			size);
	}

	void Image::CreateResource()
	{
		m_rhiDevice->MemoryCreateImage(
			&m_image,
			&m_allocation,
			m_extent,
			m_format,
			m_usage,
			m_mipLevels,
			m_samples,
			m_name.c_str());

		/*
		VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			aspect = VK_IMAGE_ASPECT_DEPTH_BIT;

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_format;
		viewInfo.subresourceRange = {
			aspect,
			0,
			m_mipLevels,
			0,
			1
		};

		VK_CHECK(vkCreateImageView(m_rhiDevice->GetDevice(),
			&viewInfo, nullptr, &m_imageView));
		*/
	}

	void Image::DestroyResource()
	{
		if (m_imageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(m_rhiDevice->GetDevice(), m_imageView, nullptr);
			m_imageView = VK_NULL_HANDLE;
		}
		if (m_image != VK_NULL_HANDLE)
		{
			m_rhiDevice->MemoryDestroyImage(m_image, m_allocation);
			m_image = VK_NULL_HANDLE;
			m_allocation = VK_NULL_HANDLE;
		}
	}
}