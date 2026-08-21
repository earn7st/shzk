#pragma once

#include "RHIDefinitions.h"

namespace shzk
{
	namespace RHIUtil
	{
		static bool IsDepthStencilFormat(RHIFormat format)
		{
			switch (format) {
			case FORMAT_D32_SFLOAT_S8_UINT:
			case FORMAT_D24_UNORM_S8_UINT:
				return true;
			default:
				return false;
			}
		}

		static bool IsDepthFormat(RHIFormat format)
		{
			switch (format) {
			case FORMAT_D32_SFLOAT:
			case FORMAT_D32_SFLOAT_S8_UINT:
			case FORMAT_D24_UNORM_S8_UINT:
				return true;
			default:
				return false;
			}
		}

		static bool IsStencilFormat(RHIFormat format)
		{
			switch (format) {
			case FORMAT_D32_SFLOAT_S8_UINT:
			case FORMAT_D24_UNORM_S8_UINT:
				return true;
			default:
				return false;
			}
		}

		static bool IsColorFormat(RHIFormat format)
		{
			return !IsDepthFormat(format) && !IsStencilFormat(format);
		}

		static bool IsRWFormat(RHIFormat format)
		{
			switch (format) {
			case FORMAT_D32_SFLOAT:
			case FORMAT_D32_SFLOAT_S8_UINT:
			case FORMAT_D24_UNORM_S8_UINT:
			case FORMAT_R8_SRGB:
			case FORMAT_R8G8_SRGB:
			case FORMAT_R8G8B8_SRGB:
			case FORMAT_R8G8B8A8_SRGB:
			case FORMAT_B8G8R8A8_SRGB:
				return false;
			default:
				return true;
			}
		}

		static uint32_t FormatToChannelCount(RHIFormat format)
		{
			switch (format) {
			case FORMAT_R8_SRGB:
			case FORMAT_R16_SFLOAT:
			case FORMAT_R32_SFLOAT:
			case FORMAT_R8_UNORM:
			case FORMAT_R16_UNORM:
			case FORMAT_R8_SNORM:
			case FORMAT_R16_SNORM:
			case FORMAT_R8_UINT:
			case FORMAT_R16_UINT:
			case FORMAT_R32_UINT:
			case FORMAT_R8_SINT:
			case FORMAT_R16_SINT:
			case FORMAT_R32_SINT:
			case FORMAT_D32_SFLOAT:
				return 1;

			case FORMAT_R8G8_SRGB:
			case FORMAT_R16G16_SFLOAT:
			case FORMAT_R32G32_SFLOAT:
			case FORMAT_R8G8_UNORM:
			case FORMAT_R16G16_UNORM:
			case FORMAT_R8G8_SNORM:
			case FORMAT_R16G16_SNORM:
			case FORMAT_R8G8_UINT:
			case FORMAT_R16G16_UINT:
			case FORMAT_R32G32_UINT:
			case FORMAT_R8G8_SINT:
			case FORMAT_R16G16_SINT:
			case FORMAT_R32G32_SINT:
			case FORMAT_D32_SFLOAT_S8_UINT:
			case FORMAT_D24_UNORM_S8_UINT:
				return 2;

			case FORMAT_R8G8B8_SRGB:
			case FORMAT_R16G16B16_SFLOAT:
			case FORMAT_R32G32B32_SFLOAT:
			case FORMAT_R8G8B8_UNORM:
			case FORMAT_R16G16B16_UNORM:
			case FORMAT_R8G8B8_SNORM:
			case FORMAT_R16G16B16_SNORM:
			case FORMAT_R8G8B8_UINT:
			case FORMAT_R16G16B16_UINT:
			case FORMAT_R32G32B32_UINT:
			case FORMAT_R8G8B8_SINT:
			case FORMAT_R16G16B16_SINT:
			case FORMAT_R32G32B32_SINT:
			case FORMAT_B10G11R11_UFLOAT:
			case FORMAT_E5B9G9R9_UFLOAT:
				return 3;

			case FORMAT_R8G8B8A8_SRGB:
			case FORMAT_B8G8R8A8_SRGB:
			case FORMAT_R16G16B16A16_SFLOAT:
			case FORMAT_R32G32B32A32_SFLOAT:
			case FORMAT_R8G8B8A8_UNORM:
			case FORMAT_R16G16B16A16_UNORM:
			case FORMAT_R8G8B8A8_SNORM:
			case FORMAT_R16G16B16A16_SNORM:
			case FORMAT_R8G8B8A8_UINT:
			case FORMAT_R16G16B16A16_UINT:
			case FORMAT_R32G32B32A32_UINT:
			case FORMAT_R8G8B8A8_SINT:
			case FORMAT_R16G16B16A16_SINT:
			case FORMAT_R32G32B32A32_SINT:
			case FORMAT_A2R10G10B10_SNORM:
			case FORMAT_A2R10G10B10_UNORM:
			case FORMAT_A2R10G10B10_SINT:
			case FORMAT_A2R10G10B10_UINT:
				return 4;

			default:
				return 0;
			}
		}

		static VkSamplerMipMapMode SamplerMipMapMode(SamplerMipMapMode mode)
		{

		}

		static VkSamplerAddressMode SamplerAddressModeToVk(SamplerAddressMode mode)
		{

		}
	}
}
