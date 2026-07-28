#pragma once

#include <memory>

namespace shzk
{
#define MAX_QUEUE_CNT 2
#define FRAMES_IN_FLIGHT 2

	class RHIQueue;
	class RHISurface;

	// Other RHI-related definitions
	enum RHIFormat : uint32_t
	{
		FORMAT_UKNOWN = 0,

		FORMAT_R8_SRGB,
		FORMAT_R8G8_SRGB,
		FORMAT_R8G8B8_SRGB,
		FORMAT_R8G8B8A8_SRGB,
		FORMAT_B8G8R8A8_SRGB,

		FORMAT_R16_SFLOAT,
		FORMAT_R16G16_SFLOAT,
		FORMAT_R16G16B16_SFLOAT,
		FORMAT_R16G16B16A16_SFLOAT,
		FORMAT_R32_SFLOAT,
		FORMAT_R32G32_SFLOAT,
		FORMAT_R32G32B32_SFLOAT,
		FORMAT_R32G32B32A32_SFLOAT,

		FORMAT_R8_UNORM,
		FORMAT_R8G8_UNORM,
		FORMAT_R8G8B8_UNORM,
		FORMAT_R8G8B8A8_UNORM,
		FORMAT_R16_UNORM,
		FORMAT_R16G16_UNORM,
		FORMAT_R16G16B16_UNORM,
		FORMAT_R16G16B16A16_UNORM,

		FORMAT_R8_SNORM,
		FORMAT_R8G8_SNORM,
		FORMAT_R8G8B8_SNORM,
		FORMAT_R8G8B8A8_SNORM,
		FORMAT_R16_SNORM,
		FORMAT_R16G16_SNORM,
		FORMAT_R16G16B16_SNORM,
		FORMAT_R16G16B16A16_SNORM,

		FORMAT_R8_UINT,
		FORMAT_R8G8_UINT,
		FORMAT_R8G8B8_UINT,
		FORMAT_R8G8B8A8_UINT,
		FORMAT_R16_UINT,
		FORMAT_R16G16_UINT,
		FORMAT_R16G16B16_UINT,
		FORMAT_R16G16B16A16_UINT,
		FORMAT_R32_UINT,
		FORMAT_R32G32_UINT,
		FORMAT_R32G32B32_UINT,
		FORMAT_R32G32B32A32_UINT,

		FORMAT_R8_SINT,
		FORMAT_R8G8_SINT,
		FORMAT_R8G8B8_SINT,
		FORMAT_R8G8B8A8_SINT,
		FORMAT_R16_SINT,
		FORMAT_R16G16_SINT,
		FORMAT_R16G16B16_SINT,
		FORMAT_R16G16B16A16_SINT,
		FORMAT_R32_SINT,
		FORMAT_R32G32_SINT,
		FORMAT_R32G32B32_SINT,
		FORMAT_R32G32B32A32_SINT,

		FORMAT_D32_SFLOAT,
		FORMAT_D32_SFLOAT_S8_UINT,
		FORMAT_D24_UNORM_S8_UINT,

		FORMAT_A2R10G10B10_SNORM,
		FORMAT_A2R10G10B10_UNORM,
		FORMAT_A2R10G10B10_SINT,
		FORMAT_A2R10G10B10_UINT,
		FORMAT_B10G11R11_UFLOAT,
		FORMAT_E5B9G9R9_UFLOAT,

		FORMAT_MAX_ENUM,
	};

	// not RHI-related structs
	typedef struct Extent2D
	{
		uint32_t width;
		uint32_t height;
	} Extent2D;


// RHI infos and related types
	enum class RHIBackendType : uint8_t
	{
		Vulkan,
		Max
	};

	typedef struct RHIInfo
	{
		RHIBackendType type = RHIBackendType::Vulkan;
		bool debug = true;
	} RHIInfo;

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
		uint32_t index;
	} RHIQueueInfo;

	typedef struct RHISwapchainInfo
	{
		std::shared_ptr<RHISurface> surface;
		std::shared_ptr<RHIQueue> presentQueue;

		uint32_t imageCount;
		Extent2D extent;
		RHIFormat format;
	} RHISwapchainInfo;

	typedef struct RHICommandPoolInfo
	{
		std::shared_ptr<RHIQueue> queue;
	} RHICommandPoolInfo;

// RHI resource definitions: infos and related types
	enum class RHIResourceType : uint8_t
	{
		Buffer,
		Texture,
		Max
	};

	typedef struct RHITextureInfo
	{

	} RHITextureInfo;

// Helper: downcast RHI abstract → concrete backend type
	template<typename Concrete, typename Abstract>
	inline Concrete* CastTo(Abstract* ptr)
	{
		return static_cast<Concrete*>(ptr);
	}

	template<typename Concrete, typename Abstract>
	inline std::shared_ptr<Concrete> CastTo(const std::shared_ptr<Abstract>& ptr)
	{
		return std::static_pointer_cast<Concrete>(ptr);
	}

}