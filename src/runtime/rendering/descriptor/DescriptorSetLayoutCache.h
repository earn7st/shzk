#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "render/descriptor/DescriptorSetLayout.h"

class DescriptorSetLayoutCache
{
public:
	static std::unique_ptr<DescriptorSetLayoutCache> Create(vk::Device device);

	DescriptorSetLayout defaultFrameLayout;
	DescriptorSetLayout defaultMaterialLayout;

	void Destroy(vk::Device device);

private:
	DescriptorSetLayoutCache() = default;
};