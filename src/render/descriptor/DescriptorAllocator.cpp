#include "DescriptorAllocator.h"

#include <memory>
#include <vulkan/vulkan.hpp>

std::unique_ptr<DescriptorAllocator>
DescriptorAllocator::Create(vk::Device device)
{
	auto allocator = std::unique_ptr<DescriptorAllocator>(
		new DescriptorAllocator());

	allocator->device = device;

	allocator->framePool = std::move(allocator->CreatePool(
		kMaxFrameSets,
		static_cast<vk::Flags<vk::DescriptorPoolCreateFlagBits>>(0),
		{
			{vk::DescriptorType::eUniformBuffer, kMaxFrameSets},
		}
	));

	allocator->materialPool = std::move(allocator->CreatePool(
		kMaxMaterialSets,
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		{
			{vk::DescriptorType::eUniformBuffer, kMaxMaterialSets},
			{vk::DescriptorType::eCombinedImageSampler, kMaxMaterialSets * 5}
		}
	));

	return allocator;
}

vk::DescriptorPool DescriptorAllocator::CreatePool(
	uint32_t maxSets,
	vk::DescriptorPoolCreateFlags flags,
	const std::vector<vk::DescriptorPoolSize>& poolSizes)
{
	vk::DescriptorPoolCreateInfo descPoolCI{};
	descPoolCI
		.setMaxSets(maxSets)
		.setFlags(flags)
		.setPoolSizes(poolSizes);
	
	return device.createDescriptorPool(descPoolCI);
}

vk::DescriptorSet DescriptorAllocator::AllocateFrameSet(vk::DescriptorSetLayout layout)
{
	vk::DescriptorSetAllocateInfo info{};
	info
		.setDescriptorPool(framePool)
		.setSetLayouts(layout);

	return device.allocateDescriptorSets(info)[0];
}

vk::DescriptorSet DescriptorAllocator::AllocateMaterialSet(vk::DescriptorSetLayout layout)
{
	vk::DescriptorSetAllocateInfo info{};
	info
		.setDescriptorPool(materialPool)
		.setSetLayouts(layout);

	return device.allocateDescriptorSets(info)[0];
}

void DescriptorAllocator::FreeMaterialSet(vk::DescriptorSet set)
{
	device.freeDescriptorSets(materialPool, set);
}

void DescriptorAllocator::Destroy()
{
	if (framePool) device.destroyDescriptorPool(framePool);
	if (materialPool) device.destroyDescriptorPool(materialPool);
}