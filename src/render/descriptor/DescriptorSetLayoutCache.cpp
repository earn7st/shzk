#include "DescriptorSetLayoutCache.h"

#include <vulkan/vulkan.hpp>

#include "render/descriptor/DescriptorSetLayout.h"

std::unique_ptr<DescriptorSetLayoutCache> 
DescriptorSetLayoutCache::Create(vk::Device device)
{
	auto cache = std::unique_ptr<DescriptorSetLayoutCache>(
		new DescriptorSetLayoutCache());
	
	cache->defaultFrameLayout = CreateDefaultFrameDescriptorLayout(device);
	cache->defaultMaterialLayout = CreateDefaultMaterialDescriptorLayout(device);

	return cache;
}

void DescriptorSetLayoutCache::Destroy(vk::Device device)
{
	if (defaultFrameLayout.descSetLayout)
		device.destroyDescriptorSetLayout(defaultFrameLayout.descSetLayout);
	if (defaultMaterialLayout.descSetLayout)
		device.destroyDescriptorSetLayout(defaultMaterialLayout.descSetLayout);
}