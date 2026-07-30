#pragma once

#include <memory>

namespace shzk
{
#define MAX_QUEUE_CNT 2
#define FRAMES_IN_FLIGHT 2

	class RHIQueue;
	class RHISurface;
	class RHIBuffer;
	class RHITexture;

// enum classes
	enum class RHIResourceType : uint32_t
	{
		Buffer,
		Texture,
		TextureView,
		Max
	};

	enum class RHIBackendType : uint32_t
	{
		Vulkan,
		Max = 0x7FFFFFFF
	};

	enum class RHIQueueType : uint32_t
	{
		Graphics,
		Compute,
		Transfer,
		Max
	};

	enum class RHIResourceState : uint32_t
	{
		Undefined,
		Common,
		Present,
		ColorAttachment,
		TransferSrc,
		TransferDst,
		VertexBuffer,
		IndexBuffer,
		DepthStencilAttachment,	// not impl
		UnorderedAccess,		// not impl
		ShaderResource,			// not impl
		IndirectArgument,		// not impl
		AccelerationStructure,	// not impl

		Max,
	};

	enum class MemoryUsage : uint32_t
	{
		Unknown = 0,
		GPUOnly = 1,		// 仅GPU使用，在VRAM显存上分配，不可绑定
		CPUOnly = 2,		// HOST_VISIBLE &&  HOST_COHERENT 及时同步，不需要flush到GPU，GPU可访问但是很慢
		CPUToGPU = 3,		// HOST_VISIBLE CPU端uncached，用于CPU端频繁进行数据写入，GPU端对数据进行读取
		GPUToCPU = 4,		// HOST_VISIBLE CPU端cached，用于被GPU写入且被CPU读取

		Max = 0x7FFFFFFF,
	};

// enums
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

		FORMAT_MAX_ENUM, 	//
	};

	enum ResourceTypeBits : uint32_t
	{
		RESOURCE_TYPE_NONE = 0x00000000,
		RESOURCE_TYPE_SAMPLER = 0x00000001,
		RESOURCE_TYPE_TEXTURE = 0x00000002,
		RESOURCE_TYPE_RW_TEXTURE = 0x00000004,
		RESOURCE_TYPE_TEXTURE_CUBE = 0x00000008,
		RESOURCE_TYPE_RENDER_TARGET = 0x00000010,
		RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER = 0x00000020,
		RESOURCE_TYPE_BUFFER = 0x00000040,
		RESOURCE_TYPE_RW_BUFFER = 0x00000080,
		RESOURCE_TYPE_UNIFORM_BUFFER = 0x00000100,
		RESOURCE_TYPE_VERTEX_BUFFER = 0x00000200,
		RESOURCE_TYPE_INDEX_BUFFER = 0x00000400,
		RESOURCE_TYPE_INDIRECT_BUFFER = 0x00000800,
		RESOURCE_TYPE_TEXEL_BUFFER = 0x00001000,
		RESOURCE_TYPE_RW_TEXEL_BUFFER = 0x00002000,
		RESOURCE_TYPE_RAY_TRACING = 0x00004000,

		RESOURCE_TYPE_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t ResourceType;

	enum BufferCreationFlagBits : uint32_t
	{
		BUFFER_CREATION_NONE = 0x00000000,
		BUFFER_CREATION_PERSISTENT_MAP = 0x00000001,
		BUFFER_CREATION_FORCE_ALIGNMENT = 0x00000002,

		BUFFER_CREATION_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t BufferCreationFlags;

	enum TextureCreationFlagBits : uint32_t
	{
		TEXTURE_CREATION_NONE = 0x00000000,
		TEXTURE_CREATION_FORCE_2D = 0x00000001,
		TEXTURE_CREATION_FORCE_3D = 0x00000002,

		TEXTURE_CREATION_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t TextureCreationFlags;

	enum TextureAspectFlagBits : uint32_t
	{
		TEXTURE_ASPECT_NONE = 0x00000000,
		TEXTURE_ASPECT_COLOR = 0x00000001,
		TEXTURE_ASPECT_DEPTH = 0x00000002,
		TEXTURE_ASPECT_STENCIL = 0x00000004,

		TEXTURE_ASPECT_DEPTH_STENCIL = TEXTURE_ASPECT_DEPTH | TEXTURE_ASPECT_STENCIL,

		TEXTURE_ASPECT_MAX_ENUM = 0x7FFFFFFF,	//
	};
	typedef uint32_t TextureAspectFlags;

// structs
	typedef struct Extent2D
	{
		uint32_t width;
		uint32_t height;
	} Extent2D;

	typedef struct Extent3D
	{
		uint32_t width;
		uint32_t height;
		uint32_t depth;
	} Extent3D;

	typedef struct TextureSubresourceRange
	{
		TextureAspectFlags	  aspect = TEXTURE_ASPECT_NONE;
		uint32_t              baseMipLevel = 0;
		uint32_t              levelCount = 0;
		uint32_t              baseArrayLayer = 0;
		uint32_t              layerCount = 0;

		uint32_t			  __padding = 0;

		friend bool operator==(const TextureSubresourceRange& a, const TextureSubresourceRange& b)
		{
			return 	a.aspect == b.aspect &&
				a.baseMipLevel == b.baseMipLevel &&
				a.levelCount == b.levelCount &&
				a.baseArrayLayer == b.baseArrayLayer &&
				a.layerCount == b.layerCount;
		}

		bool IsDefault()
		{
			return 	aspect == TEXTURE_ASPECT_NONE &&
				baseMipLevel == 0 &&
				levelCount == 0 &&
				baseArrayLayer == 0 &&
				layerCount == 0;
		}

	} TextureSubresourceRange;

	typedef struct TextureSubresourceLayers
	{
		TextureAspectFlags	  aspect = TEXTURE_ASPECT_NONE;
		uint32_t              mipLevel = 0;
		uint32_t              baseArrayLayer = 0;
		uint32_t              layerCount = 0;

		friend bool operator==(const TextureSubresourceLayers& a, const TextureSubresourceLayers& b)
		{
			return 	a.aspect == b.aspect &&
				a.mipLevel == b.mipLevel &&
				a.baseArrayLayer == b.baseArrayLayer &&
				a.layerCount == b.layerCount;
		}

		bool IsDefault()
		{
			return 	aspect == TEXTURE_ASPECT_NONE &&
				mipLevel == 0 &&
				baseArrayLayer == 0 &&
				layerCount == 0;
		}

	} TextureSubresourceLayers;

	typedef struct RHIInfo
	{
		RHIBackendType type = RHIBackendType::Vulkan;
		bool debug = true;
	} RHIInfo;

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

	typedef struct RHITextureInfo
	{
		RHIFormat format;
		Extent3D extent;
		uint32_t arrayLayers = 1;
		uint32_t mipLevels = 1;

		MemoryUsage memoryUsage = MemoryUsage::GPUOnly;
		ResourceType type		= RESOURCE_TYPE_TEXTURE;

		TextureCreationFlags creationFlag = TEXTURE_CREATION_NONE;

		// for hash
		/*
		friend bool operator== (const RHITextureInfo& a, const RHITextureInfo& b)
		{
			return  a.format == b.format &&
				a.extent == b.extent &&
				a.arrayLayers == b.arrayLayers &&
				a.mipLevels == b.mipLevels &&
				a.memoryUsage == b.memoryUsage &&
				a.type == b.type &&
				a.creationFlag == b.creationFlag;
		}*/
	} RHITextureInfo;

	
	typedef struct RHIBufferBarrier
	{
		std::shared_ptr<RHIBuffer> buffer;
		RHIResourceState srcState;
		RHIResourceState dstState;

		uint32_t offset = 0;
		uint32_t size = 0;

	} RHIBufferBarrier;
	

	typedef struct RHITextureBarrier
	{
		std::shared_ptr<RHITexture> texture;
		RHIResourceState srcState;
		RHIResourceState dstState;

		TextureSubresourceRange subresource = {};	// default of RHITexture

	} RHITextureBarrier;

// Cast
// TODO: Type Traits
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