#include "VulkanHelpers.h"
#include <vma/vk_mem_alloc.h>
#include <fstream>
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
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, 
		VMA_MEMORY_USAGE_GPU_ONLY);

	const size_t indexBufferSize = data.indices.size() * sizeof(uint32_t);
	result.indexBuffer = CreateBuffer(
		vulkanContext.allocator, 
		indexBufferSize, 
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		VMA_MEMORY_USAGE_GPU_ONLY);

	result.vertexCount = static_cast<uint32_t>(data.vertices.size());
	result.indexCount = static_cast<uint32_t>(data.indices.size());

	// Upload Data
	AllocatedBuffer staging = CreateBuffer(
		vulkanContext.allocator,
		vertexBufferSize + indexBufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
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
	DestroyBuffer(vulkanContext->allocator, geometry.vertexBuffer);
	DestroyBuffer(vulkanContext->allocator, geometry.indexBuffer);
}

// Buffer
AllocatedBuffer CreateBuffer(VmaAllocator& allocator, size_t allocSize, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo bufferInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.pNext = nullptr;
	bufferInfo.size = allocSize;

	bufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);

	VmaAllocationCreateInfo vmaAllocInfo = {};
	vmaAllocInfo.usage = memoryUsage;
	vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer newBuffer;

	VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo, &newBuffer.buffer, &newBuffer.allocation,
		&newBuffer.info));

	return newBuffer;
}

void DestroyBuffer(VmaAllocator& allocator, AllocatedBuffer& buffer)
{
	vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
}

// Image
AllocatedImage CreateImage(
	VmaAllocator allocator,
	vk::Extent3D extent,
	vk::Format format,
	vk::ImageUsageFlags flags,
	VmaMemoryUsage memUsage)
{
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = static_cast<VkFormat>(format);
	imageInfo.extent = { extent.width, extent.height, extent.depth };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = static_cast<VkImageUsageFlags>(flags);
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memUsage;

	AllocatedImage result;
	result.extent = extent;
	result.format = format;

	VkImage image;
	VK_CHECK(vmaCreateImage(allocator, &imageInfo, &allocInfo, &image,
		&result.allocation, &result.info));
	result.image = image;

	return result;
}

vk::ImageView CreateImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(image);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange
		.setAspectMask(aspectFlags)
		.setBaseMipLevel(0)
		.setLevelCount(1)
		.setBaseArrayLayer(0)
		.setLayerCount(1);

	return device.createImageView(viewInfo);
}

void DestroyImage(VmaAllocator& allocator, AllocatedImage& image)
{
	vmaDestroyImage(allocator, image.image, image.allocation);
}

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

// TransitionImageLayout
void TransitionImageLayout(
	vk::CommandBuffer cmd,
	vk::Image image,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	vk::ImageAspectFlags aspectFlags)
{
	vk::ImageMemoryBarrier2 barrier{};
	barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands);
	barrier.setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite);
	barrier.setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands);
	barrier.setDstAccessMask(
		vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead);

	barrier.setOldLayout(oldLayout);
	barrier.setNewLayout(newLayout);
	barrier.setImage(image);
	barrier.subresourceRange
		.setAspectMask(aspectFlags)
		.setBaseMipLevel(0)
		.setLevelCount(VK_REMAINING_MIP_LEVELS)
		.setBaseArrayLayer(0)
		.setLayerCount(VK_REMAINING_ARRAY_LAYERS);

	vk::DependencyInfo depInfo{};
	depInfo.setImageMemoryBarriers(barrier);

	cmd.pipelineBarrier2(depInfo);
}

// Shader
vk::ShaderModule LoadShaderModule(vk::Device device, const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		fmt::println("[Vulkan Error] Failed to open shader file: {}", filePath);
		abort();
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
	file.seekg(0);
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
	file.close();

	vk::ShaderModuleCreateInfo createInfo{};
	createInfo.setCode(buffer);

	return device.createShaderModule(createInfo);
}

// Sampler
vk::Sampler CreateSampler(
	vk::Device device,
	vk::Filter magFilter,
	vk::Filter minFilter,
	vk::SamplerAddressMode addressModeU,
	vk::SamplerAddressMode addressModeV)
{
	vk::SamplerCreateInfo samplerCI{};
	samplerCI.setMagFilter(magFilter)
		.setMinFilter(minFilter)
		.setAddressModeU(addressModeU)
		.setAddressModeV(addressModeV)
		.setAddressModeW(vk::SamplerAddressMode::eRepeat)
		.setAnisotropyEnable(false)
		.setMaxAnisotropy(1.0f)
		.setBorderColor(vk::BorderColor::eIntOpaqueBlack)
		.setUnnormalizedCoordinates(false)
		.setCompareEnable(false)
		.setCompareOp(vk::CompareOp::eAlways)
		.setMipmapMode(vk::SamplerMipmapMode::eLinear)
		.setMinLod(0.0f)
		.setMaxLod(0.0f);

	return device.createSampler(samplerCI);
}