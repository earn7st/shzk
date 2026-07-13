#pragma once

#include <vulkan/vulkan.hpp>

struct Texture
{
	AllocatedImage image;
	vk::ImageView imageView;
	vk::Sampler sampler;
};