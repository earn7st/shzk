#pragma once

#include "Asset.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <string>

namespace shzk
{
	class Shader : public Asset
	{
	public:
		Shader() = default;
		Shader(const std::string& path, ShaderFrequency frequency, const std::string& entry = "main");
		~Shader() = default;

		std::string m_path;
		std::string m_entry;

		ShaderFrequency m_frequency = SHADER_FREQUENCY_MAX_ENUM;
		std::shared_ptr<RHIShader> m_shader;
	};
}