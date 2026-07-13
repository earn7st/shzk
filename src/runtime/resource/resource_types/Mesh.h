#pragma once

#include <vulkan/vulkan.hpp>

#include "resource/ResourceHandle.h"

struct Primitive
{
	GeometryHandle geometry;
	vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
	uint32_t firstIndex = 0;
	uint32_t indexCount;

	MaterialHandle material;
};

struct Mesh
{
	std::string name;
	std::vector<Primitive> primitives;
};