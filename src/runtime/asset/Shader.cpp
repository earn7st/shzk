#include "Shader.h"

#include <string>

namespace shzk
{
	Shader::Shader(const std::string& path, ShaderFrequency frequency, const std::string& entry)
		: Asset(AssetType::Shader), m_path(path), m_frequency(frequency), m_entry(entry)
	{
		
	}
}