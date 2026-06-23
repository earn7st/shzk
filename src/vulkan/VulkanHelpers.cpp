#include "VulkanHelpers.h"

#include <vma/vk_mem_alloc.h>
#include "resource/resource_types/Geometry.h"
#include "resource/cpu_types/GeometryData.h"
#include "vulkan/VulkanContext.h"
#include "vulkan/uploadContext.h"

// Geometry
Geometry CreateGeometry(const VulkanContext& vulkanContext, const UploadContext& uploadContext, const GeometryData& data)
{
	Geometry result;
	const size_t vertexBufferSize = data.vertices.size() * sizeof(Vertex);
	result.vertexBuffer = CreateBuffer(
		vulkanContext.allocator,
		vertexBufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	const size_t indexBufferSize = data.indices.size() * sizeof(uint32_t);
	result.indexBuffer = CreateBuffer(
		vulkanContext.allocator, 
		indexBufferSize, 
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	result.vertexCount = static_cast<uint32_t>(data.vertices.size());
	result.indexCount = static_cast<uint32_t>(data.indices.size());

	// Upload Data
	AllocatedBuffer staging = CreateBuffer(
		vulkanContext.allocator,
		vertexBufferSize + indexBufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY);

	void* stagingData = staging.info.pMappedData;
	// void* stagingData = staging.allocation->GetMappedData();
	memcpy(stagingData, data.vertices.data(), vertexBufferSize);
	memcpy((char*)stagingData + vertexBufferSize, data.indices.data(), indexBufferSize);

	ImmediateSubmit(vulkanContext, uploadContext, 
		[&](vk::CommandBuffer cmd)
		{
			vk::BufferCopy vertexCopy{};
			vertexCopy.srcOffset = 0;
			vertexCopy.dstOffset = 0;
			vertexCopy.size = vertexBufferSize;

			cmd.copyBuffer(
				staging.buffer,
				result.vertexBuffer.buffer,
				vertexCopy);

			vk::BufferCopy indexCopy{};
			indexCopy.srcOffset = vertexBufferSize;
			indexCopy.dstOffset = 0;
			indexCopy.size = indexBufferSize;

			cmd.copyBuffer(
				staging.buffer,
				result.indexBuffer.buffer,
				indexCopy);
		});

	return result;
}

void DestroyGeometry(VulkanContext* vulkanContext, Geometry& geometry)
{

}

// Buffer
AllocatedBuffer CreateBuffer(VmaAllocator& allocator, size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo bufferInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.pNext = nullptr;
	bufferInfo.size = allocSize;

	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaAllocInfo = {};
	vmaAllocInfo.usage = memoryUsage;
	vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer newBuffer;

	VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo, &newBuffer.buffer, &newBuffer.allocation,
		&newBuffer.info));

	return newBuffer;
}

// Image


// Immediate Submit
void ImmediateSubmit(const VulkanContext& vulkanContext, const UploadContext& uploadContext, std::function<void(vk::CommandBuffer cmd)>&& function)
{
	VK_CHECK(vulkanContext.device.waitForFences(
		uploadContext.uploadFence,
		true,
		UINT64_MAX));

	vulkanContext.device.resetFences(uploadContext.uploadFence);
	vulkanContext.device.resetCommandPool(uploadContext.commandPool);

	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	uploadContext.commandBuffer.begin(beginInfo);

	function(uploadContext.commandBuffer);

	uploadContext.commandBuffer.end();

	vk::SubmitInfo submitInfo{};

	submitInfo.setCommandBufferCount(1).setPCommandBuffers(&uploadContext.commandBuffer);

	vulkanContext.graphicsQueue.submit(submitInfo, uploadContext.uploadFence);

	VK_CHECK(vulkanContext.device.waitForFences(uploadContext.uploadFence, true, UINT64_MAX));
}