#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace vkR::rhi
{
	class Device;

	enum class RHIBufferType
	{
		Vertex,
		Index,
		//Instance,
		Storage,
		//Constant,
		//ShaderBindingTable,
		Max
	};

	class Buffer
	{
	public:
		Buffer() = delete;
		Buffer(
			Device* device,
			RHIBufferType type,
			uint32_t stride,
			uint32_t elementCount,
			bool mappable,
			const void* data,
			const char* name);
		~Buffer();

		void UploadData(const void* data);

		VkBuffer GetBuffer() const { return m_buffer; }
	
	private:
		void CreateResource();
		void DestroyResource();

		VkBufferUsageFlags    GetUsageFlags()     const;
		VkMemoryPropertyFlags GetMemoryFlags()    const;

	private:
		RHIBufferType	m_type			= RHIBufferType::Max;
		uint32_t		m_stride		= 0;
		uint32_t		m_elementCount	= 0;
		uint32_t		m_offset		= 0;
		std::string		m_name;
		bool			m_mappable = false;

		Device*			m_rhiDevice		= nullptr;

		VkBuffer		m_buffer		= VK_NULL_HANDLE;
		VmaAllocation	m_allocation	= nullptr;
		void*			m_mappedData	= nullptr;
	};
}
