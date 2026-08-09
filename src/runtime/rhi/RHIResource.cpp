#include "RHIResource.h"
#include "runtime/log/Log.h"

#include <algorithm>

namespace shzk
{
	Extent3D RHITexture::MipExtent(uint32_t mipLevel)
	{
		if (mipLevel > m_info.mipLevels)
		{
			SHZK_LOG_ERROR("Mip level exceed the texture's maximum mip levels");
			return Extent3D{};
		}

		Extent3D extent = m_info.extent;
		extent.width = std::max((uint32_t)1, extent.width >> mipLevel);
		extent.height = std::max((uint32_t)1, extent.height >> mipLevel);
		extent.depth = std::max((uint32_t)1, extent.depth >> mipLevel);

		return extent;
	}
}