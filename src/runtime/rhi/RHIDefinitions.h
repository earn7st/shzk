#pragma once

#include <iostream>

namespace shzk
{
#define MAX_QUEUE_CNT 2

	enum class RHIQueueType : uint8_t
	{
		Graphics,
		Compute,
		Transfer,
		Max
	};

	typedef struct RHIQueueInfo
	{
		RHIQueueType type;
		// uint32_t index;
	}RHIQueueInfo;

	enum class RHIResourceType : uint8_t
	{
		Buffer,
		Texture,
		Max
	};

	typedef struct Extent2D
	{
		uint32_t width;
		uint32_t height;
	} Extent2D;

}