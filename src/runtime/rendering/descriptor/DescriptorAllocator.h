#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

class DescriptorAllocator
{
public:
	static std::unique_ptr<DescriptorAllocator> Create(vk::Device device);

	vk::DescriptorSet AllocateFrameSet(vk::DescriptorSetLayout layout);
	vk::DescriptorSet AllocateMaterialSet(vk::DescriptorSetLayout layout);

	void Destroy();

	void FreeMaterialSet(vk::DescriptorSet set);

private:
	DescriptorAllocator() = default;

	vk::Device device;

	static constexpr uint32_t kMaxFrameSets = 4;
	vk::DescriptorPool framePool;

	static constexpr uint32_t kMaxMaterialSets = 1024;
	vk::DescriptorPool materialPool;

	vk::DescriptorPool CreatePool(
		uint32_t maxSets,
		vk::DescriptorPoolCreateFlags flags,
		const std::vector<vk::DescriptorPoolSize>& poolSizes);
};