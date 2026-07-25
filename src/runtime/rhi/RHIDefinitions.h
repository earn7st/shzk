#pragma once

#include <iostream>

namespace shzk
{
	enum class RHIBackendType : uint8_t
	{
		Vulkan,
		Max
	};

	typedef struct RHIInfo
	{
		RHIBackendType type = RHIBackendType::Vulkan;
		bool debug = true;
	}RHIInfo;

	enum class RHIQueueType : uint8_t
	{
		Graphics,
		Compute,
		Transfer,
		Max
	};

#define MAX_QUEUE_CNT 2
	typedef struct RHIQueueInfo
	{
		RHIQueueType type;
		uint32_t index;
	}RHIQueueInfo;

	struct RHISwapchainInfo
	{

	};

	struct RHICommandPoolInfo
	{

	};

// RHI resource definitions

	enum class RHIResourceType : uint8_t
	{
		Buffer,
		Texture,
		Max
	};

// Other structs
	typedef struct Extent2D
	{
		uint32_t width;
		uint32_t height;
	} Extent2D;


	
}