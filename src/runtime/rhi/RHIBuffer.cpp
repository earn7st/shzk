#include "RHIBuffer.h"
#include "RHIDevice.h"
#include "RHIUtil.h"

#include <cassert>
#include <vma/vk_mem_alloc.h>

namespace vkR::rhi
{
	Buffer::Buffer(
		Device* device,
		RHIBufferType type,
		uint32_t stride,
		uint32_t elementCount,
		bool mappable,
		const void* data,
		const char* name)
	{
		m_rhiDevice = device;
		m_type = type;
		m_stride = stride;
		m_elementCount = elementCount;
		m_mappable = mappable;
		m_name = name;

		CreateResource(data);
	}

	Buffer::~Buffer() { DestroyResource(); }

	void Buffer::UploadData(const void* data)
	{
		if (!data) return;

		const size_t size = m_stride * m_elementCount;

		if (m_mappable)
		{
			assert(m_mappedData);
			memcpy(m_mappedData, data, size);
		}
		else
		{
			m_rhiDevice->UploadDataToBuffer(m_buffer, data, size);
		}
	}

	void Buffer::CreateResource(const void* data)
	{
		const VkDeviceSize size = static_cast<VkDeviceSize>(m_stride) * m_elementCount;
		const VkBufferUsageFlags usage = GetUsageFlags();
		const VkMemoryPropertyFlags memFlags = GetMemoryFlags();

		m_rhiDevice->MemoryCreateBuffer(
			&m_buffer,
			&m_allocation,
			&m_mappedData,
			size,
			usage,
			memFlags,
			m_name.c_str());

		assert(m_buffer != VK_NULL_HANDLE);
		if (m_mappable)
		{
			assert(m_mappedData != nullptr);
		}

		if (data)
		{
			UploadData(data);
		}
	}

	void Buffer::DestroyResource()
	{
		if (!m_buffer) return;
		vmaDestroyBuffer(m_rhiDevice->GetAllocator(), m_buffer, m_allocation);
		m_buffer = VK_NULL_HANDLE;
		m_allocation = VK_NULL_HANDLE;
		m_mappedData = nullptr;
	}

	VkBufferUsageFlags Buffer::GetUsageFlags() const
	{
		switch (m_type)
		{
		case RHIBufferType::Vertex:
			return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		case RHIBufferType::Index:
			return VK_BUFFER_USAGE_INDEX_BUFFER_BIT
				| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		case RHIBufferType::Storage:
			return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		default:
			return 0;
		}
	}

	VkMemoryPropertyFlags Buffer::GetMemoryFlags() const
	{
		if (m_mappable)
		{
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		else
		{
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
	}
}