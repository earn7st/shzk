#pragma once

#include <iostream>

namespace shzk
{
	enum class RHIResourceType : uint32_t
	{
		Buffer,
		Texture
	};

	typedef struct Extent2D
	{
		uint32_t width;
		uint32_t height;
	} Extent2D;

}