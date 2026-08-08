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

	static std::string AssetTypeToString(const AssetType& type)
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

		std::string GetAssetTypeName() const { return AssetTypeToString(m_type); }
		AssetType GetAssetType() const { return m_type; }
		void SetName(const std::string& name) { m_name = name; };
		inline const std::string& GetName() const { return m_name; }

	protected:
		AssetType m_type	= AssetType::Unknown;
		std::string m_name	=  "Untitled";
		// TODO: uid;
	};
}