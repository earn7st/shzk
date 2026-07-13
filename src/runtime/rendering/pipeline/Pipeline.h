#pragma once

#include <vulkan/vulkan.hpp>

struct Pipeline
{
	vk::Pipeline pipeline;
	vk::PipelineLayout pipelineLayout;

	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
};

Pipeline CreateDefaultPipeline(
	vk::Device device,
	vk::DescriptorSetLayout frameLayout,
	vk::DescriptorSetLayout materialLayout,
	vk::Format colorFormat,
	vk::Format depthFormat);