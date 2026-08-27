#include "Material.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/render/resources/RenderResourceManager.h"
#include "runtime/rhi/RHIResource.h"

namespace shzk
{
	void Material::InitRenderResources()
	{
		m_buffer = std::make_shared<Buffer<MaterialUniformShaderParameters>>();

		m_descriptorSet = RenderResourceManager::Get()->CreateMaterialDescriptorSet();
		if (!m_descriptorSet)
		{
			SHZK_LOG_ERROR("Material DescriptorSet Init Failed, check RenderResourceManager Init!");
			assert(false);
			return;
		}
		m_descriptorSet->UpdateBuffer(MATERIAL_BINDING_UNIFORM, m_buffer->GetBuffer());
	}

	void Material::UpdateUniformData()
	{
		MaterialUniformShaderParameters ub;
		ub.baseColor = m_baseColor;
		ub.emission = m_emission;
		ub.metallic = m_metallic;
		ub.roughness = m_roughness;
		ub.alphaCutoff = m_alphaCutoff;
		ub.bUseVertexColor = m_bUseVertexColor ? 1 : 0;
		for (int i = 0; i < 8; ++i)
		{
			ub.ints[i] = m_ints[i];
			ub.floats[i] = m_floats[i];
			ub.colors[i] = m_colors[i];
		}
		m_buffer->SetData(ub);
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