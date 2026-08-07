#pragma once

#include "Asset.h"

#include <string>

namespace shzk
{
	class Material : public Asset
	{
	public:
		Material() : Asset(AssetType::Material) {}
		~Material() = default;

		inline const std::string& GetName() const { return m_name; }

	private:
		std::string m_name;
	};
}