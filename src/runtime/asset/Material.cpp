#include "Material.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/render/resources/RenderResourceManager.h"
#include "runtime/rhi/RHIResource.h"

namespace shzk
{
	void Material::InitRenderResources()
	{
		m_descriptorSet = RenderResourceManager::Get()->CreateMaterialDescriptorSet();
		if (!m_descriptorSet)
		{
			SHZK_LOG_ERROR("Material DescriptorSet Init Failed, check RenderResourceManager Init!");
			assert(false);
			return;
		}
		m_descriptorSet->UpdateBuffer(MATERIAL_BINDING_UNIFORM, m_buffer.GetBuffer());
	}

	void Material::UpdateUniformData()
	{
		MaterialUniformData data;
		data.baseColor = m_baseColor;
		data.emission = m_emission;
		data.metallic = m_metallic;
		data.roughness = m_roughness;
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

	void Material::UpdateTexture(uint32_t binding, std::shared_ptr<Texture> texture)
	{
		if (!texture || !texture->m_textureView || !m_descriptorSet) 
		{
			SHZK_LOG_WARN("Material::UpdateTexture failed, resources not adequate!");
			return;
		}

		m_descriptorSet->UpdateTexture(binding, texture->m_textureView, RenderResourceManager::Get()->GetDefaultSampler()->m_sampler);
	}
}