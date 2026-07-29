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
	}
}
