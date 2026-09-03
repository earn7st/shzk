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

		inline std::string GetPath() const { return m_path; }
		inline std::string GetEntry() const { return m_entry; }

		inline ShaderFrequency GetFrequency() const { return m_frequency; }
		inline std::shared_ptr<RHIShader> GetRHIShader() const { return m_shader; }

	private:
		std::string m_path;
		std::string m_entry;

		ShaderFrequency m_frequency = SHADER_FREQUENCY_MAX_ENUM;
		std::shared_ptr<RHIShader> m_shader = nullptr;
	
		// void LoadFromFile();	 // User Defined Shaders
	};
}