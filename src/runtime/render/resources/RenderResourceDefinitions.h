#pragma once

#include <cstdint>

namespace shzk
{
	enum DescriptorSetIndex : uint32_t
	{
		DESCRIPTORSET_INDEX_PER_FRAME = 0,
		DESCRIPTORSET_INDEX_MATERIAL = 1,

		DESCRIPTORSET_INDEX_MAX, 
	};

	enum PerFrameBinding : uint32_t
	{

	};

	enum MaterialBinding : uint32_t
	{
		MATERIAL_BINDING_UNIFORM = 0,   // uniform buffer: Material Data
		MATERIAL_BINDING_DIFFUSE = 1,
		MATERIAL_BINDING_NORMAL = 2,
		MATERIAL_BINDING_ARM = 3,
		MATERIAL_BINDING_SPECULAR = 4,
		MATERIAL_BINDING_TEXTURE2D = 5,   // [5, 13)  m_texture2D[8]
		MATERIAL_BINDING_TEXTURECUBE = 13,  // [13, 17) m_textureCube[4]
		MATERIAL_BINDING_TEXTURE3D = 17,  // [17, 21) m_texture3D[4]
	};
}