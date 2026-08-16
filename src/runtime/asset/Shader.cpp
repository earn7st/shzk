#include "Shader.h"
#include "runtime/render/resources/RenderResourceManager.h"

#include <string>

namespace shzk
{
	Shader::Shader(const std::string& path, ShaderFrequency frequency, const std::string& entry)
		: Asset(AssetType::Shader), m_path(path), m_frequency(frequency), m_entry(entry)
	{
		m_shader = RenderResourceManager::Get()->GetOrCreateRHIShader(m_path, m_frequency, m_entry);
	}
}