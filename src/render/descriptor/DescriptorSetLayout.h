#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

class DescriptorSetLayout
{
public:
	vk::DescriptorSetLayout descSetLayout;
	std::vector<vk::DescriptorSetLayoutBinding> bindings;

	void AddBinding(uint32_t index, vk::DescriptorType descType, vk::ShaderStageFlags shaderStage);
};

DescriptorSetLayout CreateDefaultFrameDescriptorLayout(vk::Device device);
DescriptorSetLayout CreateDefaultMaterialDescriptorLayout(vk::Device device);