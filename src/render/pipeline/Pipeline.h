#pragma once

#include <vulkan/vulkan.hpp>

struct Pipeline
{
	vk::Pipeline pipeline;
	vk::PipelineLayout pipelineLayout;

	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
};

Pipeline CreateWhitePipeline(
	vk::Device device,
	vk::DescriptorSetLayout frameLayout,
	vk::DescriptorSetLayout materialLayout,
	vk::Format colorFormat,
	vk::Format depthFormat);