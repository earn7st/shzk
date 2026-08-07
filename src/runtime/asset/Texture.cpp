#include "Texture.h"
#include "Texture.h"
#include "Texture.h"
#include "Asset.h"
#include "runtime/log/Log.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIUtil.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <stb_image.h>
#include <filesystem>

namespace shzk
{
	TextureViewType TextureTypeToTextureViewType(TextureType type)
	{
		TextureViewType viewType;
		switch (type) {
		case TextureType::Type2D:           viewType = TextureViewType::View2D;       break;
		case TextureType::Type2DArray:      viewType = TextureViewType::View2DArray;  break;
		case TextureType::TypeCube:         viewType = TextureViewType::ViewCube;     break;
		case TextureType::Type3D:           viewType = TextureViewType::View3D;       break;
		default:							SHZK_LOG_ERROR("Unsupported texture type!");
		}
		return viewType;
	}

	Texture::Texture(std::string path, TextureType type = TextureType::Type2D)
		: Asset(AssetType::Texture), m_type(type), m_format(RHIFormat::FORMAT_R8G8B8A8_SRGB), m_arrayLayer(1)
	{
		m_paths.push_back(std::filesystem::path(path).filename().string());
	}

	void Texture::LoadFromFile()
	{
		if (m_type == TextureType::TypeCube && m_paths.size() != 6)
		{
			SHZK_LOG_ERROR("File paths num incorrect for texture type CUBE");
			return;
		}
		
		for (size_t i = 0; i < m_paths.size(); ++i)
		{
			std::string& path = m_paths[i];
			int width, height, channels;
			stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
			if (!pixels)
			{
				SHZK_LOG_ERROR("Texture::LoadFromFile ¡ª failed to load: {}", path);
				return;
			}

			if (!m_rhiInitialized)	// for TextureType::Cube, RHITexture should only be created once
			{
				m_extent = { (uint32_t)width, (uint32_t)height, 1 };
				m_mipLevels = (uint32_t)(std::floor(std::log2(std::max(width, height)))) + 1;
				InitRHI();

				m_rhiInitialized = true;
			}
		}

		// upload, staging buffer, immediate
		
	}

	void shzk::Texture::InitRHI()
	{
		ResourceType resourceType = (m_type == TextureType::TypeCube) ? (RESOURCE_TYPE_TEXTURE_CUBE | RESOURCE_TYPE_TEXTURE) : RESOURCE_TYPE_TEXTURE;
		if (RHIUtil::IsRWFormat(m_format))      resourceType |= RESOURCE_TYPE_RW_TEXTURE;
		if (RHIUtil::IsRWFormat(m_format))      resourceType |= RESOURCE_TYPE_RENDER_TARGET;

		TextureAspectFlags aspects = RHIUtil::IsDepthStencilFormat(m_format) ? TEXTURE_ASPECT_DEPTH_STENCIL :
			RHIUtil::IsDepthFormat(m_format) ? TEXTURE_ASPECT_DEPTH :
			RHIUtil::IsStencilFormat(m_format) ? TEXTURE_ASPECT_STENCIL : TEXTURE_ASPECT_COLOR;

		bool force2D = m_extent.width == 1 && m_extent.height == 1;

		RHITextureInfo textureInfo = {
			.format = m_format,
			.extent = m_extent,
			.arrayLayers = m_arrayLayer,
			.mipLevels = m_mipLevels,
			.memoryUsage = MemoryUsage::GPUOnly,
			.type = resourceType,
			.creationFlag = force2D ? TEXTURE_CREATION_FORCE_2D : TEXTURE_CREATION_NONE };
		m_texture = RHI::Get()->CreateTexture(textureInfo);

		RHITextureViewInfo textureViewInfo = {
			.texture = m_texture,
			.format = m_format,
			.viewType = TextureTypeToTextureViewType(m_type),
			.subresourceRange = { aspects, 0, m_mipLevels, 0, m_arrayLayer} };
		m_textureView = RHI::Get()->CreateTextureView(textureViewInfo);

	}
}