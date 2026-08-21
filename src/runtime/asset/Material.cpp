#include "Material.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/render/resources/RenderResourceManager.h"

namespace shzk
{
	void Material::InitRenderResources()
	{
		m_descriptorSet = RenderResourceManager::Get()->CreateMaterialDescriptorSet();

		m_buffer = Buffer<MaterialUniformData>();
		
	}

	void Material::UpdateUniformData()
	{
		MaterialUniformData data;
		data.baseColor = m_baseColor;
		data.emission = m_emission;
		data.metallic = m_roughness;
		data.alphaCutoff = m_alphaCutoff;
		data.bUseVertexColor = m_bUseVertexColor ? 1 : 0;
		for (int i = 0; i < 8; ++i)
		{
			data.ints[i] = m_ints[i];
			data.floats[i] = m_floats[i];
			data.colors[i] = m_colors[i];
		}

		m_buffer.SetData(data);
	}
}