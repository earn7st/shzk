#pragma once

#include <memory>
#include <string>
#include <vector>

namespace shzk
{
#if WIN32
	#define MAX_QUEUE_CNT 2
#else
	#define MAX_QUEUE_CNT 1
#endif 

#define FRAMES_IN_FLIGHT 2

	class RHIQueue;
	class RHISurface;
	class RHISwapchain;
	class RHIBuffer;
	class RHITexture;
	class RHITextureView;
	class RHIShader;
	class RHIDescriptorSet;
	class RHIRootSignature;

// enum classes
	enum class RHIResourceType : uint32_t
	{
		Buffer,
		Texture,
		TextureView,
		Sampler,
		Shader,

		RootSignature,
		DescriptorSet,

		GraphicsPipeline,

		Max,
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
		Unknown,
		GPUOnly,		// 仅GPU使用，在VRAM显存上分配，不可绑定
		CPUOnly,		// HOST_VISIBLE &&  HOST_COHERENT 及时同步，不需要flush到GPU，GPU可访问但是很慢
		CPUToGPU,		// HOST_VISIBLE CPU端uncached，用于CPU端频繁进行数据写入，GPU端对数据进行读取
		GPUToCPU,		// HOST_VISIBLE CPU端cached，用于被GPU写入且被CPU读取

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
		FORMAT_B8G8R8A8_UNORM,
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

	enum ResourceTypeBits : uint32_t
	{
		RESOURCE_TYPE_NONE						= 0,
		RESOURCE_TYPE_SAMPLER					= 1 << 0,
		RESOURCE_TYPE_TEXTURE					= 1 << 1,
		RESOURCE_TYPE_RW_TEXTURE				= 1 << 2,
		RESOURCE_TYPE_TEXTURE_CUBE				= 1 << 3,
		RESOURCE_TYPE_RENDER_TARGET				= 1 << 4,
		RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER	= 1 << 5,
		RESOURCE_TYPE_BUFFER					= 1 << 6,
		RESOURCE_TYPE_RW_BUFFER					= 1 << 7,
		RESOURCE_TYPE_UNIFORM_BUFFER			= 1 << 8,
		RESOURCE_TYPE_VERTEX_BUFFER				= 1 << 9,
		RESOURCE_TYPE_INDEX_BUFFER				= 1 << 10,
		RESOURCE_TYPE_INDIRECT_BUFFER			= 1 << 11,
		RESOURCE_TYPE_TEXEL_BUFFER				= 1 << 12,
		RESOURCE_TYPE_RW_TEXEL_BUFFER			= 1 << 13,
		// RESOURCE_TYPE_RAY_TRACING				= 1 << 14,

		RESOURCE_TYPE_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t ResourceType;

	enum BufferCreationFlagBits : uint32_t
	{
		BUFFER_CREATION_NONE					= 0,
		BUFFER_CREATION_PERSISTENT_MAP			= 1 << 0,
		BUFFER_CREATION_FORCE_ALIGNMENT			= 1 << 1,

		BUFFER_CREATION_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t BufferCreationFlags;

	enum TextureCreationFlagBits : uint32_t
	{
		TEXTURE_CREATION_NONE		= 0,
		TEXTURE_CREATION_FORCE_2D	= 1 << 0,
		TEXTURE_CREATION_FORCE_3D	= 1 << 1,

		TEXTURE_CREATION_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t TextureCreationFlags;

	enum TextureAspectFlagBits : uint32_t
	{
		TEXTURE_ASPECT_NONE		= 0,
		TEXTURE_ASPECT_COLOR	= 1 << 0,
		TEXTURE_ASPECT_DEPTH	= 1 << 1,
		TEXTURE_ASPECT_STENCIL	= 1 << 2,

		TEXTURE_ASPECT_DEPTH_STENCIL = TEXTURE_ASPECT_DEPTH | TEXTURE_ASPECT_STENCIL,

		TEXTURE_ASPECT_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t TextureAspectFlags;

	enum class TextureViewType : uint32_t
	{
		View1D,
		View2D,
		View3D,
		ViewCube,
		View1DArray,
		View2DArray,
		ViewCubeArray,
		Max,
	};

	enum ShaderFrequencyBits : uint32_t
	{
		SHADER_FREQUENCY_COMPUTE		= 1 << 0,
		SHADER_FREQUENCY_VERTEX			= 1 << 1,
		SHADER_FREQUENCY_FRAGMENT		= 1 << 2,
		SHADER_FREQUENCY_GEOMETRY		= 1 << 3,
		SHADER_FREQUENCY_RAY_GEN		= 1 << 4,
		SHADER_FREQUENCY_CLOSEST_HIT	= 1 << 5,
		SHADER_FREQUENCY_RAY_MISS		= 1 << 6,
		SHADER_FREQUENCY_INTERSECTION	= 1 << 7,
		SHADER_FREQUENCY_ANY_HIT		= 1 << 8,
		SHADER_FREQUENCY_MESH			= 1 << 9,

		SHADER_FREQUENCY_GRAPHICS = SHADER_FREQUENCY_VERTEX |
		SHADER_FREQUENCY_FRAGMENT |
		SHADER_FREQUENCY_GEOMETRY |
		SHADER_FREQUENCY_MESH,
		SHADER_FREQUENCY_RAY_TRACING = SHADER_FREQUENCY_RAY_GEN |
		SHADER_FREQUENCY_CLOSEST_HIT |
		SHADER_FREQUENCY_RAY_MISS |
		SHADER_FREQUENCY_INTERSECTION |
		SHADER_FREQUENCY_ANY_HIT,
		SHADER_FREQUENCY_ALL = SHADER_FREQUENCY_GRAPHICS |
		SHADER_FREQUENCY_COMPUTE |
		SHADER_FREQUENCY_RAY_TRACING,

		SHADER_FREQUENCY_MAX_ENUM = 0x7FFFFFFF,
	};
	typedef uint32_t ShaderFrequency;

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
		TextureAspectFlags	  aspect			= TEXTURE_ASPECT_NONE;
		uint32_t              baseMipLevel		= 0;
		uint32_t              levelCount		= 0;
		uint32_t              baseArrayLayer	= 0;
		uint32_t              layerCount		= 0;

		uint32_t			  __padding			= 0;

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
		TextureAspectFlags	  aspect			= TEXTURE_ASPECT_NONE;
		uint32_t              mipLevel			= 0;
		uint32_t              baseArrayLayer	= 0;
		uint32_t              layerCount		= 0;

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

	typedef struct ShaderResourceEntry
	{
		uint32_t set = 0;
		uint32_t binding = 0;
		uint32_t size = 1;
		ShaderFrequency frequency = SHADER_FREQUENCY_ALL;

		ResourceType type = RESOURCE_TYPE_NONE;

		friend bool operator== (const ShaderResourceEntry& a, const ShaderResourceEntry& b)
		{
			return  a.set == b.set &&
				a.binding == b.binding &&
				a.size == b.size &&
				a.frequency == b.frequency &&
				a.type == b.type;
		}
	} ShaderResourceEntry;

	typedef struct PushConstantsInfo
	{
		uint32_t offset = 0;
		uint32_t size = 128;
		ShaderFrequency frequency = SHADER_FREQUENCY_ALL;
		friend bool operator== (const PushConstantsInfo& a, const PushConstantsInfo& b)
		{
			return  a.offset == b.offset &&
				a.size == b.size &&
				a.frequency == b.frequency;
		}
	} PushConstantsInfo;

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

	typedef struct RHIBufferInfo
	{
		uint64_t size;

		MemoryUsage memoryUsage = MemoryUsage::GPUOnly;
		ResourceType type = RESOURCE_TYPE_BUFFER;

		BufferCreationFlags creationFlag = BUFFER_CREATION_NONE;

	} RHIBufferInfo;

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

	typedef struct RHITextureViewInfo
	{
		std::shared_ptr<RHITexture> texture;
		RHIFormat format = FORMAT_UKNOWN;
		TextureViewType viewType = TextureViewType::View2D;

		TextureSubresourceRange subresourceRange{}; // default
	} RHITextureViewInfo;
	
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

	typedef struct RHISamplerInfo
	{

	} RHISamplerInfo;

	typedef struct RHIShaderInfo
	{
		std::string entry = "main";

		ShaderFrequency frequency;
		std::vector<uint8_t> code;
	} RHIShaderInfo;

	typedef struct RHIRootSignatureInfo
	{
		std::vector<ShaderResourceEntry> entries;
		std::vector<PushConstantsInfo> pushConstants;


	} RHIRootSignatureInfo;

	typedef struct RHIGraphicsPipelineInfo
	{
		std::shared_ptr<RHIShader> vertexShader;
		std::shared_ptr<RHIShader> fragmentShader;
		
		std::shared_ptr<RHIRootSignature> rootSignature;



	} RHIGraphicsPipelineInfo;

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