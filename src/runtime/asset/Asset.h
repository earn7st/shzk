#pragma once

#include <string>

namespace shzk
{
	enum class AssetType
	{
		Unknown,
		Model,
		Texture,
		Shader,
		Material,
	};

	std::string AssetTypeToString(const AssetType& type)
	{
		switch (type)
		{
		case AssetType::Unknown:	return "Unknown";
		case AssetType::Model:		return "Model";
		case AssetType::Texture:	return "Texture";
		case AssetType::Shader:		return "Shader";
		case AssetType::Material:	return "Material";
		default:					return "Unknown";
		}
	}

	class Asset
	{
	public:
		Asset() = default;
		Asset(AssetType type) : m_type(type) {}
		virtual ~Asset() = default;

		void SetName(std::string name) { m_name = name; };

		std::string GetTypeName() const { return AssetTypeToString(m_type); }
		AssetType GetType() const { return m_type; }

	protected:
		AssetType m_type	= AssetType::Unknown;
		std::string m_name	=  "Untitled";
		// TODO: uid;
	};
}